//
// Created by Nikita Kasinski
//

#pragma once

#include <expected>
#include "result_codes.h"
#include "menu/menu.h"
#include "concurrent_writer.h"
#include "controller.h"
#include "client_server_options.h"

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
        Quit = static_cast<int>(ClientServerOptions::Quit),
        Read = static_cast<int>(ClientServerOptions::Read),
        Modify = static_cast<int>(ClientServerOptions::Modify),
        Last,
    };

    // methods

    bool isValidOptionCode(std::size_t rawEnumValue, std::size_t rawLastEnumValue) const override;

    std::expected<std::unique_ptr<MenuOption>, ResultCode> createMenuOption(std::size_t rawEnumValue) const override;

    [[nodiscard]] ResultCode initializeOption() override;

    std::expected<std::size_t, ResultCode> getOptionCode() const override;

    ResultCode handleInitializationError(ResultCode initializationError) const override;

    ResultCode handleExecutionError(ResultCode executionError) const override;
    
    // fields

    std::shared_ptr<ConcurrentWriter> _writer;
    std::shared_ptr<HANDLE> _pipe;
    const int _threadId;
    std::shared_ptr<Controller> _ctrl;
    std::shared_ptr<std::vector<std::size_t>> _recordAccessReadCount;
    std::shared_ptr<CRITICAL_SECTION> _acs;
};