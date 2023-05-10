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
    auto smartPipe = clientMenu->getPipe();
    HANDLE unwrappedPipe = SmartWinapi::unwrap(smartPipe);
    DWORD bytes;

    int key = -1;
    out << clientMenu->keyPrompt;
    in >> key;

    WriteFile(unwrappedPipe, &Protocol::READ, Protocol::SIZE, &bytes, NULL);
    WriteFile(unwrappedPipe, reinterpret_cast<char *>(&key), sizeof(int), &bytes, NULL);

    out << "Waiting for record to become available for reading\n";

    char response;
    ReadFile(unwrappedPipe, &response, Protocol::SIZE, &bytes, NULL);

    if (Protocol::FAILURE == response)
    {
        out << "There is no employee under such id\n";
        return ResultCode::EmployeeNotFound;
    }
    else if (Protocol::SUCCESS == response)
    {
        out << "Access granted\n";

        Employee employeeRead;
        ReadFile(unwrappedPipe, reinterpret_cast<char *>(&employeeRead), sizeof(Employee), &bytes, NULL);
        out << "Employee received\n";
        Utility::printEmployee(out, employeeRead);
        out << "Enter any key to continue\n";
        char x;
        in >> x;
        WriteFile(unwrappedPipe, &Protocol::FINISH, Protocol::SIZE, &bytes, NULL);
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
    return MenuOption::isQuitOption();
}