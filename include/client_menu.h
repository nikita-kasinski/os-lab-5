//
// Created by Nikita Kasinski
//
#pragma once

#include <iostream>
#include "menu.h"

class ClientOptionExit : public MenuOption
{
public:
    [[nodiscard]] ResultCode execute() const override;
    bool isQuitOption() const override;
};

class ClientMenu : public Menu
{
public:
    ClientMenu(const std::shared_ptr<HANDLE> &pipe, std::ostream &out, std::istream &in);
    [[nodiscard]] ResultCode start() override;

private:
    [[nodiscard]] ResultCode initializeOption() override;
    std::shared_ptr<HANDLE> _pipe;
    std::ostream &_out;
    std::istream &_in;
};