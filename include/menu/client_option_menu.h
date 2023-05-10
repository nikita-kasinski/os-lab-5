//
// Created by Nikita Kasinski
//

#pragma once

#include "menu/client_menu.h"

class ClientMenu::ClientOptionMenu : public MenuOption
{
public:
    ClientOptionMenu(const ClientMenu *menu);

    [[nodiscard]] ResultCode execute() override;

    bool isQuitOption() const override;
};