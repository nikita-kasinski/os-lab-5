//
// Created by Nikita Kasinski
//
#pragma once

#include <memory>
#include <result_codes.h>
#include <expected>

class Menu;
class MenuOption;

class Menu
{
public:
    [[nodiscard]] virtual ResultCode start() = 0;

protected:
    class MenuOption;

    Menu();

    virtual std::expected<std::unique_ptr<MenuOption>, ResultCode> createMenuOption(int rawEnumValue) const = 0;

    [[nodiscard]] virtual ResultCode initializeOption() = 0;

    std::string virtual getMenu() const = 0;

    std::unique_ptr<MenuOption> _option;
};

class Menu::MenuOption
{
public:
    MenuOption();

    MenuOption(const std::shared_ptr<Menu> &menu);

    // default implementation returns ResultCode::OK
    [[nodiscard]] virtual ResultCode execute() = 0;

    // default implementation returns false
    virtual bool isQuitOption() const = 0;

protected:
    std::shared_ptr<Menu> _menu;
};