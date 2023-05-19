//
// Created by Nikita Kasinski
//

#include "menu/menu.h"
#include "result_codes.h"

Menu::MenuOption::MenuOption(const Menu *menu) : _menu(menu)
{
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

bool Menu::isValidOptionCode(std::size_t rawEnumValue, std::size_t rawLastEnumValue) const
{
    if (rawEnumValue < 0)
    {
        return false;
    }
    if (rawEnumValue >= rawLastEnumValue)
    {
        return false;
    }
    return true;
}

ResultCode Menu::initializeOption()
{
    auto expected_optionCode = getOptionCode();
    if (expected_optionCode)
    {
        int optionCode = expected_optionCode.value();
        try
        {
            auto expected_option = createMenuOption(optionCode);
            if (expected_option)
            {
                _option = std::move(expected_option.value());
            }
            else
            {
                return expected_option.error();
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
    }
    else
    {
        return expected_optionCode.error();
    }
    return ResultCode::OK;
}