//
// Created by Nikita Kasinski
//

#pragma once

#include <expected>
#include "result_codes.h"
#include "menu/menu.h"
#include "concurrent_writer.h"
#include "controller.h"

class ServerMenu : public Menu
{
public:
    ServerMenu(
        const std::shared_ptr<HANDLE> &pipe,
        int threadId, 
        const std::shared_ptr<Controller> &ctrl,
        const std::shared_ptr<std::vector<std::size_t>> &recordAccessReadCount,
        const std::shared_ptr<CRITICAL_SECTION> &iocs,
        const std::shared_ptr<CRITICAL_SECTION> &acs);

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

    std::expected<int, ResultCode> getOptionCode() const override;

    // fields

    std::shared_ptr<ConcurrentWriter> _writer;
    std::shared_ptr<HANDLE> _pipe;
    const int _threadId;
    std::shared_ptr<Controller> _ctrl;
    std::shared_ptr<std::vector<std::size_t>> _recordAccessReadCount;
    std::shared_ptr<CRITICAL_SECTION> _acs;
};