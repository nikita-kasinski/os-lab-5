//
// Created by Nikita Kasinski
//

#pragma once

#include <expected>
#include "result_codes.h"
#include "menu/menu.h"
#include "concurrent_writer.h"

class ServerMenu : public Menu
{
public:
    ServerMenu(const std::shared_ptr<HANDLE> &pipe, const std::shared_ptr<ConcurrentWriter> &writer);

    [[nodiscard]] ResultCode start() override;

private:
    class ServerOptionQuit;
    class ServerOptionRead;
    class ServerOptionModify;

    enum class Options
    {   
        Quit,
        Read,
        Modify,
        Last,
    };

    // methods

    bool isValidOptionCode(int rawEnumValue, int rawLastEnumValue) const override;

    std::expected<std::unique_ptr<MenuOption>, ResultCode> createMenuOption(int rawEnumValue) const override;

    [[nodiscard]] ResultCode initializeOption() override;

    // fields

    std::shared_ptr<ConcurrentWriter> _writer;
    std::shared_ptr<HANDLE> _pipe;
};