//
// Created by Nikita Kasinski
//

#pragma once

#include "menu/server_menu.h"

class ServerMenu::ServerOptionQuit : public MenuOption
{
public:
    ServerOptionQuit(const Menu *menu);

    [[nodiscard]] ResultCode execute() override;

    bool isQuitOption() const override;
};