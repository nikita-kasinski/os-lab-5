//
// Created by Nikita Kasinski
//
#include "client_menu.h"
#include <iostream>

ResultCode ClientOptionExit::execute() const
{
    return MenuOption::execute();
}

bool ClientOptionExit::isQuitOption() const
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
    try
    {
        _option = std::make_unique<ClientOptionExit>();
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