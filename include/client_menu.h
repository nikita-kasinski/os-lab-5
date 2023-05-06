//
// Created by Nikita Kasinski
//
#pragma once

#include <iostream>
#include <windows.h>
#include "menu.h"

class ClientOptionQuit : public MenuOption
{
public:
    ClientOptionQuit(const std::shared_ptr<Menu> &menu);

    [[nodiscard]] ResultCode execute() override;

    bool isQuitOption() const override;
};

class ClientMenu : public Menu
{
public:

    ClientMenu(const std::shared_ptr<HANDLE> &pipe, std::ostream &out, std::istream &in);

    [[nodiscard]] ResultCode start() override;

private:
    enum class Options
    {
        Quit,
        ReadRecord,
        WriteRecord,
    };

    virtual std::expected<std::unique_ptr<MenuOption>, ResultCode> createMenuOption(int rawEnumValue) const override;

    [[nodiscard]] ResultCode initializeOption() override;

    std::shared_ptr<HANDLE> _pipe;
    std::ostream &_out;
    std::istream &_in;
};