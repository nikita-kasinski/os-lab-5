//
// Created by Nikita Kasinski
//

#include "menu/client_option_read.h"
#include "smart_winapi.h"
#include "protocol.h"
#include "employee.h"
#include "utility.h"

ClientMenu::ClientOptionReadRecord::ClientOptionReadRecord(const ClientMenu *menu) : MenuOption(menu)
{
}

ResultCode ClientMenu::ClientOptionReadRecord::execute()
{
    auto clientMenu = dynamic_cast<const ClientMenu *>(_menu);
    std::ostream &out = clientMenu->getOutStream();
    std::istream &in = clientMenu->getInStream();
    auto pipe = clientMenu->getPipe();

    int key = -1;
    out << clientMenu->keyPrompt;
    in >> key;

    ResultCode writeResult, readResult;

    writeResult = SmartWinapi::writePipe(pipe, ClientServerOptions::Read);
    if (ResultCode::OK != writeResult)
    {
        return writeResult;
    }

    writeResult = SmartWinapi::writePipe(pipe, key);
    if (ResultCode::OK != writeResult)
    {
        return writeResult;
    }

    out << "Waiting for record to become available for reading\n";

    Protocol::PROTOCOL response;
    readResult = SmartWinapi::readPipe(pipe, response);
    if (ResultCode::OK != readResult)
    {
        return readResult;
    }

    if (Protocol::FAILURE == response)
    {
        out << "There is no employee under such id\n";
        return ResultCode::EmployeeNotFound;
    }
    else if (Protocol::SUCCESS == response)
    {
        out << "Access granted\n";

        Employee employeeRead;
        readResult = SmartWinapi::readPipe(pipe, employeeRead);
        if (ResultCode::OK != readResult)
        {
            return readResult;
        }

        out << "Employee received\n";
        Utility::printEmployee(out, employeeRead);
        out << "Enter any key to continue\n";
        char x;
        in >> x;
        writeResult = SmartWinapi::writePipe(pipe, Protocol::FINISH);
        if (ResultCode::OK != writeResult)
        {
            return writeResult;
        }
    }
    else
    {
        out << "Protocol violation. Abort\n";
        return ResultCode::ProtocolViolation;
    }
    return ResultCode::OK;
}

bool ClientMenu::ClientOptionReadRecord::isQuitOption() const
{
    return false;
}