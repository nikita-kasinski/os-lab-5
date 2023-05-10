//
// Created by Nikita Kasinski
//

#include "menu/menu.h"
#include "result_codes.h"

Menu::MenuOption::MenuOption(const Menu *menu) : _menu(menu)
{
}

bool Menu::MenuOption::isQuitOption() const
{
    return false;
}

ResultCode Menu::MenuOption::execute()
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
    // TODO add menu printing here which requires field with stream out
    while (not started or not _option->isQuitOption())
    {
        started = true;

        ResultCode initializeResult = initializeOption();
        /*if (ResultCode::OK != initializeResult)
            return initializeResult;
        */

        ResultCode executeResult = _option->execute();
        /*if (ResultCode::OK != executeResult)
            return executeResult;
        */
    }
    return ResultCode::OK;
}