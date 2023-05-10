//
// Created by Nikita Kasinski
//

#pragma once

#include "menu/server_menu.h"

class ServerMenu::ServerOptionModify : public MenuOption
{
public:
    ServerOptionModify(const Menu *menu);

    [[nodiscard]] ResultCode execute() override;

    bool isQuitOption() const override;
};