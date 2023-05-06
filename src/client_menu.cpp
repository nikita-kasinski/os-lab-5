//
// Created by Nikita Kasinski
//
#include "client_menu.h"
#include <iostream>
#include "utility.h"

ClientOptionQuit::ClientOptionQuit(const std::shared_ptr<Menu> &menu):
_menu(menu)
{
    
}

ResultCode ClientOptionQuit::execute()
{
    return MenuOption::execute();
}

bool ClientOptionQuit::isQuitOption() const
{
    return true;
}

ClientMenu::ClientMenu(const std::shared_ptr<HANDLE> &pipe, std::ostream &out, std::istream &in) : 
    Menu(), _pipe(pipe), _out(out), _in(in)
{

}

ResultCode ClientMenu::start()
{
    return Menu::start();
}

ResultCode ClientMenu::initializeOption()
{
    // TO DO make function to create menu
    std::string menu =
        "Enter corresponding request: \n"
        "   0 to see menu\n"
        "   1 to quit\n"
        "   2 to read record\n"
        "   3 to modify record\n";
    _out << menu;
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
            //TODO add error handling once error is established
        }
    }
    catch(std::bad_alloc&)
    {
        return ResultCode::BadAlloc;
    }
    catch(...)
    {
        return ResultCode::UnrecognizedInitializationError;
    }
    return ResultCode::OK;
}

std::expected<std::unique_ptr<MenuOption>, ResultCode> ClientMenu::createMenuOption(int rawEnumValue) const
{
    
}