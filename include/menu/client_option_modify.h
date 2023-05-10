//
// Created by Nikita Kasinski
//

#pragma once

#include "menu/client_menu.h"

class ClientMenu::ClientOptionModifyRecord : public MenuOption
{
public:
    ClientOptionModifyRecord(const ClientMenu *menu);

    [[nodiscard]] ResultCode execute() override;

    bool isQuitOption() const override;
};