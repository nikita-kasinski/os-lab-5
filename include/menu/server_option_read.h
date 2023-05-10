//
// Created by Nikita Kasinski
//

#pragma once

#include "menu/server_menu.h"

class ServerMenu::ServerOptionRead : public MenuOption
{
public:
    ServerOptionRead(const Menu *menu);

    [[nodiscard]] ResultCode execute() override;

    bool isQuitOption() const override;
};