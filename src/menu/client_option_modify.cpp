//
// Created by Nikita Kasinski
//

#include "menu/client_option_modify.h"
#include "protocol.h"
#include "smart_winapi.h"
#include "employee.h"
#include "utility.h"

ClientMenu::ClientOptionModifyRecord::ClientOptionModifyRecord(const ClientMenu *menu) : MenuOption(menu)
{
}

ResultCode ClientMenu::ClientOptionModifyRecord::execute()
{
    auto clientMenu = dynamic_cast<const ClientMenu*>(_menu);
    std::ostream &out = clientMenu->getOutStream();
    std::istream &in = clientMenu->getInStream();
    auto smartPipe = clientMenu->getPipe();
    HANDLE unwrappedPipe = SmartWinapi::unwrap(smartPipe);
    DWORD bytes;

    int key = -1;
    out << clientMenu->keyPrompt;
    in >> key;

    WriteFile(unwrappedPipe, &Protocol::MODIFY, Protocol::SIZE, &bytes, NULL);
    WriteFile(unwrappedPipe, reinterpret_cast<char *>(&key), sizeof(int), &bytes, NULL);

    out << "Waiting for record to become available for modifying\n";

    char response;
    ReadFile(unwrappedPipe, &response, Protocol::SIZE, &bytes, NULL);

    if (Protocol::SUCCESS == response)
    {
        out << "Access granted\n";

        // reading old record
        Employee employeeRead;
        ReadFile(unwrappedPipe, reinterpret_cast<char *>(&employeeRead), sizeof(Employee), &bytes, NULL);

        // printing old record
        out << "Old record\n";
        Utility::printEmployee(out, employeeRead);

        // recieving new record
        out << "Enter new values for this record\n";
        employeeRead = Utility::readEmployee(in, out);

        // writing rew record to pipe
        WriteFile(unwrappedPipe, reinterpret_cast<char *>(&employeeRead), sizeof(Employee), &bytes, NULL);

        // reading response
        char response;
        ReadFile(unwrappedPipe, &response, Protocol::SIZE, &bytes, NULL);

        if (Protocol::SUCCESS == response)
        {
            out << "The record was changed successfully\n";
            out << "Enter any key to continue\n";
            char x;
            in >> x;
            WriteFile(unwrappedPipe, &Protocol::FINISH, Protocol::SIZE, &bytes, NULL);
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