//
// Created by Nikita Kasinski
//
#include "client_menu.h"
#include <iostream>
#include "utility.h"
#include "smart_winapi.h"
#include "protocol.h"

ClientMenu::ClientOptionQuit::ClientOptionQuit(const ClientMenu *menu) : MenuOption(menu)
{
}

ResultCode ClientMenu::ClientOptionQuit::execute()
{
    return MenuOption::execute();
}

bool ClientMenu::ClientOptionQuit::isQuitOption() const
{
    return true;
}

ClientMenu::ClientOptionMenu::ClientOptionMenu(const ClientMenu *menu) : MenuOption(menu)
{
}

ResultCode ClientMenu::ClientOptionMenu::execute()
{
    auto clientMenu = dynamic_cast<const ClientMenu *>(_menu);
    std::string menuString = clientMenu->getMenu();
    std::ostream &out = clientMenu->getOutStream();
    out << menuString;
    return ResultCode::OK;
}

bool ClientMenu::ClientOptionMenu::isQuitOption() const
{
    return Menu::MenuOption::isQuitOption();
}

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
    return MenuOption::isQuitOption();
}

ClientMenu::ClientMenu(const std::shared_ptr<HANDLE> &pipe, std::ostream &out, std::istream &in) : Menu(), _pipe(pipe), _out(out), _in(in)
{
}

ResultCode ClientMenu::start()
{
    return Menu::start();
}

ResultCode ClientMenu::initializeOption()
{
    size_t request = Utility::safeUnsignedIntegerInput(_in, _out, "Enter request: ", "Value must be non negative integer\n");
    try
    {
        auto expected_option = createMenuOption(request);
        if (expected_option)
        {
            _option = std::move(expected_option.value());
        }
        else
        {
            // TODO add error handling once error is established
        }
    }
    catch (std::bad_alloc &)
    {
        return ResultCode::BadAlloc;
    }
    catch (...)
    {
        return ResultCode::UnrecognizedInitializationError;
    }
    return ResultCode::OK;
}

std::expected<std::unique_ptr<Menu::MenuOption>, ResultCode> ClientMenu::createMenuOption(int rawEnumValue) const
{
    if (!isValidOptionCode(rawEnumValue))
    {
        return std::unexpected(ResultCode::UnrecognizedOption);
    }

    ClientMenu::Options optionCode = static_cast<ClientMenu::Options>(rawEnumValue);

    switch (optionCode)
    {
    case ClientMenu::Options::Menu:
    {
        return std::make_unique<ClientOptionMenu>(this);
        break;
    }
    case ClientMenu::Options::Quit:
    {
        return std::make_unique<ClientOptionQuit>(this);
        break;
    }
    case ClientMenu::Options::ReadRecord:
    {
        return std::make_unique<ClientOptionReadRecord>(this);
        break;
    }
    case ClientMenu::Options::ModifyRecord:
    {
        return std::make_unique<ClientOptionModifyRecord>(this);
        break;
    }
    case ClientMenu::Options::Last:
    {
        // code shouldn't reach here
        return std::unexpected(ResultCode::UnreachableCodeReached);
        break;
    }
    }
    // neither should reach here
    return std::unexpected(ResultCode::UnreachableCodeReached);
}

bool ClientMenu::isValidOptionCode(int rawEnumValue) const
{
    if (rawEnumValue < 0)
    {
        return false;
    }
    if (rawEnumValue >= static_cast<int>(ClientMenu::Options::Last))
    {
        return false;
    }
    return true;
}

std::shared_ptr<HANDLE> ClientMenu::getPipe() const
{
    return _pipe;
}

std::istream &ClientMenu::getInStream() const
{
    return _in;
}

std::ostream &ClientMenu::getOutStream() const
{
    return _out;
}

std::string ClientMenu::getMenu() const
{
    // TODO make function dynamic from options enum
    std::string menu =
        "Enter corresponding request: \n"
        "   0 to see menu\n"
        "   1 to quit\n"
        "   2 to read record\n"
        "   3 to modify record\n";
    return menu;
}