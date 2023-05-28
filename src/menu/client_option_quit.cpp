//
// Created by Nikita Kasinski
//

#include "menu/client_option_quit.h"
#include "menu/client_server_options.h"
#include "smart_winapi.h"
#include "protocol.h"

ClientMenu::ClientOptionQuit::ClientOptionQuit(const ClientMenu *menu) : MenuOption(menu)
{
}

ResultCode ClientMenu::ClientOptionQuit::execute()
{
    auto clientMenu = dynamic_cast<const ClientMenu *>(_menu);
    auto pipe = clientMenu->getPipe();

    auto writeResult = SmartWinapi::writePipe(pipe, ClientServerOptions::Quit);
    
    if (ResultCode::OK != writeResult)
    {
        return writeResult;
    }
    
    return ResultCode::OK;
}

bool ClientMenu::ClientOptionQuit::isQuitOption() const
{
    return true;
}