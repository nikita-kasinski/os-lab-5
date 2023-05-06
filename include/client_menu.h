//
// Created by Nikita Kasinski
//
#pragma once

#include "menu.h"

class ClientOptionExit: public MenuOption
{
public:
    ResultCode execute() const override;
    bool isQuitOption() const override;
};

class ClientMenu: public Menu
{
protected:
    ResultCode initializeOption() override;
public:
    ClientMenu();
    ResultCode start() override;
};