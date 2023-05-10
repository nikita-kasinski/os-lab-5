//
// Created by Nikita Kasinski
//

#include "menu/client_menu.h"

class ClientMenu::ClientOptionReadRecord : public MenuOption
{
public:
    ClientOptionReadRecord(const ClientMenu *menu);

    [[nodiscard]] ResultCode execute() override;

    bool isQuitOption() const override;
};