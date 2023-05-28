//
// Created by Nikita Kasinski
//
#include "menu/client_menu.h"
#include <iostream>
#include "utility.h"
#include "smart_winapi.h"
#include "protocol.h"
#include "menu/client_option_menu.h"
#include "menu/client_option_quit.h"
#include "menu/client_option_read.h"
#include "menu/client_option_modify.h"

ClientMenu::ClientMenu(const std::shared_ptr<HANDLE> &pipe, std::ostream &out, std::istream &in) : Menu(), _pipe(pipe), _out(out), _in(in)
{
}

ResultCode ClientMenu::start()
{
    return Menu::start();
}

ResultCode ClientMenu::initializeOption()
{
    return Menu::initializeOption();
}

std::expected<std::unique_ptr<Menu::MenuOption>, ResultCode> ClientMenu::createMenuOption(std::size_t rawEnumValue) const
{
    if (!isValidOptionCode(rawEnumValue, static_cast<int>(ClientMenu::Options::Last)))
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

bool ClientMenu::isValidOptionCode(std::size_t rawEnumValue, std::size_t rawLastEnumValue) const
{
    return Menu::isValidOptionCode(rawEnumValue, rawLastEnumValue);
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

std::string ClientMenu::getMenuPrompt() const
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

std::expected<size_t, ResultCode> ClientMenu::getOptionCode() const
{
    return Utility::safeUnsignedIntegerInput(_in, _out, "Enter request: ", "Value must be non negative integer\n");
}

ResultCode ClientMenu::handleInitializationError(ResultCode initializationError) const
{
    return Menu::handleInitializationError(initializationError);
}

ResultCode ClientMenu::handleExecutionError(ResultCode executionError) const
{
    return Menu::handleExecutionError(executionError);
}