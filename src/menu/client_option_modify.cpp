//
// Created by Nikita Kasinski
//

#include "menu/client_option_modify.h"
#include "protocol.h"
#include "smart_winapi.h"
#include "employee.h"
#include "utility.h"
#include "menu/client_server_options.h"

ClientMenu::ClientOptionModifyRecord::ClientOptionModifyRecord(const ClientMenu *menu) : MenuOption(menu)
{
}

ResultCode ClientMenu::ClientOptionModifyRecord::execute()
{
    auto clientMenu = dynamic_cast<const ClientMenu*>(_menu);
    std::ostream &out = clientMenu->getOutStream();
    std::istream &in = clientMenu->getInStream();
    auto pipe = clientMenu->getPipe();

    int key = -1;
    out << clientMenu->keyPrompt;
    in >> key;
    
    ResultCode writeResult, readResult;

    writeResult = SmartWinapi::writePipe(pipe, ClientServerOptions::Modify);
    if (ResultCode::OK != writeResult)
    {
        return writeResult;
    }

    writeResult = SmartWinapi::writePipe(pipe, key);
    if (ResultCode::OK != writeResult)
    {
        return writeResult;
    }

    out << "Waiting for record to become available for modifying\n";

    Protocol::PROTOCOL response;
    readResult = SmartWinapi::readPipe(pipe, response);
    if (ResultCode::OK != readResult)
    {
        return readResult;
    }

    if (Protocol::SUCCESS == response)
    {
        out << "Access granted\n";

        // reading old record
        Employee employeeRead;
        readResult = SmartWinapi::readPipe(pipe, employeeRead);
        if (ResultCode::OK != readResult)
        {
            return readResult;
        }

        // printing old record
        out << "Old record\n";
        Utility::printEmployee(out, employeeRead);

        // recieving new record
        out << "Enter new values for this record\n";
        employeeRead = Utility::readEmployee(in, out);

        // writing rew record to pipe
        writeResult = SmartWinapi::writePipe(pipe, employeeRead);
        if (ResultCode::OK != writeResult)
        {
            return writeResult;
        }

        // reading response
        Protocol::PROTOCOL response;
        readResult = SmartWinapi::readPipe(pipe, response);
        if (ResultCode::OK != readResult)
        {
            return readResult;
        }

        if (Protocol::SUCCESS == response)
        {
            out << "The record was changed successfully\n";
            out << "Enter any key to continue\n";
            char x;
            in >> x;
            writeResult = SmartWinapi::writePipe(pipe, Protocol::FINISH);
            if (ResultCode::OK != writeResult)
            {
                return writeResult;
            }
        }
        else if (Protocol::FAILURE == response)
        {
            out << "The id of new record is already taken\n";
            return ResultCode::EmployeeIdTaken;
        }
        else
        {
            out << "Protocol violation. Abort\n";
            return ResultCode::ProtocolViolation;
        }
    }
    else if (Protocol::FAILURE == response)
    {
        out << "No record under such id\n";
        return ResultCode::EmployeeNotFound;
    }
    else
    {
        out << "Protocol violation. Abort\n";
        return ResultCode::ProtocolViolation;
    }
    return ResultCode::OK;
}

bool ClientMenu::ClientOptionModifyRecord::isQuitOption() const
{
    return false;
}