//
// Created by Nikita Kasinski
//

#include "menu/server_option_quit.h"

ServerMenu::ServerOptionQuit::ServerOptionQuit(const Menu *menu) : MenuOption(menu)
{
}

ResultCode ServerMenu::ServerOptionQuit::execute()
{
    const ServerMenu *serverMenu = dynamic_cast<const ServerMenu *>(_menu);
    auto pipe = serverMenu->_pipe;
    HANDLE unwrappedPipe = SmartWinapi::unwrap(pipe);

    auto disconnectResult = DisconnectNamedPipe(unwrappedPipe);

    if (!disconnectResult)
    {
        return ResultCode::PipeDisconnectError;
    }
    return ResultCode::OK;
}

bool ServerMenu::ServerOptionQuit::isQuitOption() const
{
    return true;
}
