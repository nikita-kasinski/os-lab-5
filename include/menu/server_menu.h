//
// Created by Nikita Kasinski
//

#pragma once

#include <expected>
#include "result_codes.h"
#include "menu/menu.h"


class ServerMenu : public Menu
{
public:
    ServerMenu();

    [[nodiscard]] ResultCode start() override;

private:
    class ServerOptionQuit;
    class ServerOptionRead;
    class ServerOptionModify;

    enum class Options
    {   
        Quit,
        Read,
        Modify,
        Last,
    };

    bool isValidOptionCode(int rawEnumValue) const override;

    std::expected<std::unique_ptr<MenuOption>, ResultCode> createMenuOption(int rawEnumValue) const override;

    [[nodiscard]] ResultCode initializeOption() override;

};