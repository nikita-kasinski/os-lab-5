//
// Created by Nikita Kasinski
//

#include "menu/client_menu.h"

class ClientMenu::ClientOptionQuit : public MenuOption
{
public:
    ClientOptionQuit(const ClientMenu *menu);

    [[nodiscard]] ResultCode execute() override;

    bool isQuitOption() const override;
};