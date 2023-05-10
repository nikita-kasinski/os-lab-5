//
// Created by Nikita Kasinski
//

#include "menu/client_option_menu.h"

ClientMenu::ClientOptionMenu::ClientOptionMenu(const ClientMenu *menu) : MenuOption(menu)
{
}

ResultCode ClientMenu::ClientOptionMenu::execute()
{
    auto clientMenu = dynamic_cast<const ClientMenu *>(_menu);
    std::string menuString = clientMenu->getMenuPrompt();
    std::ostream &out = clientMenu->getOutStream();
    out << menuString;
    return ResultCode::OK;
}

bool ClientMenu::ClientOptionMenu::isQuitOption() const
{
    return false;
}