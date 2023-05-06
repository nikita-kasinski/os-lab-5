//
// Created by Nikita Kasinski
// 
#pragma once

#include <memory>

class MenuOption
{
public:
    [[nodiscard]] virtual int execute() const = 0;
    virtual bool isQuitOption() const = 0;
};

class Menu
{
private:
    std::unique_ptr<MenuOption> option;
public:
    [[nodiscard]] virtual int initializeOption() = 0;
    [[nodiscard]] virtual int start() = 0;
};