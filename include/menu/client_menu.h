//
// Created by Nikita Kasinski
//
#pragma once

#include <iostream>
#include <windows.h>
#include "menu.h"

class ClientMenu : public Menu
{
public:
    ClientMenu(const std::shared_ptr<HANDLE> &pipe, std::ostream &out, std::istream &in);

    [[nodiscard]] ResultCode start() override;

private:
    class ClientOptionQuit;
    class ClientOptionMenu;
    class ClientOptionReadRecord;
    class ClientOptionModifyRecord;

    enum class Options
    {
        Menu,
        Quit,
        ReadRecord,
        ModifyRecord,
        Last,
    };

    bool isValidOptionCode(int rawEnumValue, int rawLastEnumValue) const override;

    std::expected<std::unique_ptr<MenuOption>, ResultCode> createMenuOption(int rawEnumValue) const override;

    [[nodiscard]] ResultCode initializeOption() override;

    std::shared_ptr<HANDLE> getPipe() const;

    std::istream &getInStream() const;

    std::ostream &getOutStream() const;

    std::string getMenuPrompt() const;

    std::shared_ptr<HANDLE> _pipe;
    std::ostream &_out;
    std::istream &_in;

    const std::string keyPrompt = "Input employee id to access its record: ";
};