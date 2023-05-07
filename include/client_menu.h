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

    enum class Options
    {
        Menu,
        Quit,
        ReadRecord,
        ModifyRecord,
        Last,
    };

    bool isValidOptionCode(int rawEnumValue) const;

    virtual std::expected<std::unique_ptr<MenuOption>, ResultCode> createMenuOption(int rawEnumValue) const override;

    [[nodiscard]] ResultCode initializeOption() override;

    std::shared_ptr<HANDLE> getPipe() const;

    std::istream &getInStream() const;

    std::ostream &getOutStream() const;

    std::string getMenu() const override;

    std::shared_ptr<HANDLE> _pipe;
    std::ostream &_out;
    std::istream &_in;
};

class ClientMenu::ClientOptionQuit : public MenuOption
{
public:
    ClientOptionQuit(const ClientMenu *menu);

    [[nodiscard]] ResultCode execute() override;

    bool isQuitOption() const override;
};

class ClientMenu::ClientOptionMenu : public MenuOption
{
public:
    ClientOptionMenu(const ClientMenu *menu);

    [[nodiscard]] ResultCode execute() override;

    bool isQuitOption() const override;
};

class ClientMenu::ClientOptionReadRecord : public MenuOption
{
public:
    ClientOptionReadRecord(const ClientMenu *menu);

    [[nodiscard]] ResultCode execute() override;

    bool isQuitOption() const override;
};