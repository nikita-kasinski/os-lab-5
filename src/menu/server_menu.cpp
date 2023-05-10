//
// Created by Nikita Kasinski
//

#include <memory>
#include "menu/server_menu.h"
#include "menu/server_option_modify.h"
#include "menu/server_option_quit.h"
#include "menu/server_option_read.h"

ServerMenu::ServerMenu(const std::shared_ptr<HANDLE> &pipe, const std::shared_ptr<ConcurrentWriter> &writer) : 
    Menu(), _writer(writer), _pipe(pipe)
{
}

ResultCode ServerMenu::start()
{
    return Menu::start();
}

bool ServerMenu::isValidOptionCode(int rawEnumValue, int rawLastEnumValue) const
{
    return Menu::isValidOptionCode(rawEnumValue, rawLastEnumValue);
}

std::expected<std::unique_ptr<Menu::MenuOption>, ResultCode> ServerMenu::createMenuOption(int rawEnumValue) const
{
    if (!isValidOptionCode(rawEnumValue, static_cast<int>(ServerMenu::Options::Last)))
    {
        return std::unexpected(ResultCode::UnrecognizedOption);
    }

    Options serverOption = static_cast<Options>(rawEnumValue);

    switch(serverOption)
    {
        case Options::Quit:
        {
            return std::make_unique<ServerOptionQuit>(this);
            break;
        }
        case Options::Read:
        {
            return std::make_unique<ServerOptionRead>(this);
            break;
        }
        case Options::Modify:
        {
            return std::make_unique<ServerOptionModify>(this);
        }
        case Options::Last:
        {
            // shouldn't reach here
            return std::unexpected(ResultCode::UnreachableCodeReached);
        }
    }
    // neither here
    return std::unexpected(ResultCode::UnreachableCodeReached);
}

std::expected<int, ResultCode> ServerMenu::getOptionCode() const
{
    HANDLE pipe = SmartWinapi::unwrap(_pipe);
    size_t result;

    DWORD bytes;
    auto readResult = ReadFile(SmartWinapi::unwrap(pipe), &result, sizeof(char), &bytes, NULL);
    if (readResult != TRUE)
    {
        return std::unexpected(ResultCode::PipeReadError);
    }
    if (bytes != sizeof(char))
    {
        return std::unexpected(ResultCode::PipeReadInvalidSize);
    }

    return result;
}

ResultCode ServerMenu::initializeOption()
{
    return Menu::initializeOption();
}