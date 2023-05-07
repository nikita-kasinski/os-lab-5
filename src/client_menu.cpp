//
// Created by Nikita Kasinski
//
#include "client_menu.h"
#include <iostream>
#include "utility.h"

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

ClientMenu::ClientMenu(const std::shared_ptr<HANDLE> &pipe, std::ostream &out, std::istream &in) : Menu(), _pipe(pipe), _out(out), _in(in)
{
}

ResultCode ClientMenu::start()
{
    return Menu::start();
}

ResultCode ClientMenu::initializeOption()
{
    size_t request = Utility::safeUnsignedIntegerInput(_in, _out, "Enter request: ", "Value must be positive integer\n");
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
        // TODO add return statement with ClientOptionReadRecord
        break;
    }
    case ClientMenu::Options::ModifyRecord:
    {
        // TODO add return statement with ClientOptionModifyRecord
        break;
    }
    case ClientMenu::Options::Last:
    {
        // code shouldn't reach here
        return std::unexpected(ResultCode::UnreachableCodeReached);
        break;
    }
    }
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