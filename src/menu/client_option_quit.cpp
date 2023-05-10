//
// Created by Nikita Kasinski
//

#include "menu/client_option_quit.h"
#include "smart_winapi.h"
#include "protocol.h"

ClientMenu::ClientOptionQuit::ClientOptionQuit(const ClientMenu *menu) : MenuOption(menu)
{
}

ResultCode ClientMenu::ClientOptionQuit::execute()
{
    auto clientMenu = dynamic_cast<const ClientMenu *>(_menu);
    auto smartPipe = clientMenu->getPipe();
    HANDLE unwrappedPipe = SmartWinapi::unwrap(smartPipe);
    DWORD bytes;

    WriteFile(unwrappedPipe, &Protocol::QUIT, Protocol::SIZE, &bytes, NULL);

    return ResultCode::OK;
}

bool ClientMenu::ClientOptionQuit::isQuitOption() const
{
    return true;
}