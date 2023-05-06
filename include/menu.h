//
// Created by Nikita Kasinski
//
#pragma once

#include <memory>
#include <result_codes.h>
#include <expected>

class MenuOption
{
public:
    MenuOption();
    
    MenuOption(const std::shared_ptr<Menu> &menu);

    [[nodiscard]] virtual ResultCode execute() = 0;

    virtual bool isQuitOption() const = 0;

protected:
    std::shared_ptr<Menu> _menu;
};

class Menu
{
public:
    [[nodiscard]] virtual ResultCode start() = 0;

protected:
    Menu();

    virtual std::expected<std::shared_ptr<MenuOption>, ResultCode> createMenuOption(int rawEnumValue) const = 0;

    [[nodiscard]] virtual ResultCode initializeOption() = 0;

    std::unique_ptr<MenuOption> _option;
};