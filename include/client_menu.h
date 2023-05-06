//
// Created by Nikita Kasinski
//
#pragma once

#include "menu.h"

class ClientOptionExit: public MenuOption
{
public:
    [[nodiscard]] ResultCode execute() const override;
    bool isQuitOption() const override;
};

class ClientMenu: public Menu
{
protected:
    [[nodiscard]] ResultCode initializeOption() override;
public:
    ClientMenu();
    [[nodiscard]] ResultCode start() override;
};