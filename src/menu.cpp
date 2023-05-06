//
// Created by Nikita Kasinski
//

#include "menu.h"
#include "result_codes.h"

MenuOption::MenuOption() : _menu(std::shared_ptr<Menu>())
{
}
MenuOption::MenuOption(const std::shared_ptr<Menu> &menu) : _menu(menu)
{
}

ResultCode MenuOption::execute()
{
    return ResultCode::OK;
}

Menu::Menu()
{
    _option = std::unique_ptr<MenuOption>();
}
ResultCode Menu::start()
{
    bool started = false;
    while (not started or not _option->isQuitOption())
    {
        started = true;

        ResultCode initializeResult = initializeOption();
        if (ResultCode::OK != initializeResult)
            return initializeResult;

        ResultCode executeResult = _option->execute();
        if (ResultCode::OK != executeResult)
            return executeResult;
    }
    return ResultCode::OK;
}