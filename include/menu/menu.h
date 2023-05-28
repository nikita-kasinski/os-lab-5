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

    virtual bool isValidOptionCode(std::size_t rawEnumValue, std::size_t rawLastEnumValue) const = 0;

    virtual std::expected<std::unique_ptr<MenuOption>, ResultCode> createMenuOption(std::size_t rawEnumValue) const = 0;

    virtual std::expected<std::size_t, ResultCode> getOptionCode() const = 0;

    [[nodiscard]] virtual ResultCode initializeOption() = 0;

    virtual ResultCode handleInitializationError(ResultCode initializationError) const = 0;

    virtual ResultCode handleExecutionError(ResultCode executionError) const = 0;

    std::unique_ptr<MenuOption> _option;
};

class Menu::MenuOption
{
public:

    MenuOption(const Menu *menu);

    [[nodiscard]] virtual ResultCode execute() = 0;

    virtual bool isQuitOption() const = 0;

protected:
    const Menu *_menu = nullptr;
};