//
// Created by Nikita Kasinski
// 
#pragma once

#include <memory>
#include <result_codes.h>

class MenuOption
{
public:
    [[nodiscard]] virtual ResultCode execute() const = 0;
    virtual bool isQuitOption() const = 0;
};

class Menu
{
private:
    std::unique_ptr<MenuOption> option;
public:
    [[nodiscard]] virtual ResultCode initializeOption() = 0;
    [[nodiscard]] virtual ResultCode start() = 0;
};