//
// Created by Nikita Kasinski
//

#include "menu/server_menu.h"

ServerMenu::ServerMenu() : Menu()
{
}

ResultCode ServerMenu::start()
{
    return Menu::start();
}

bool ServerMenu::isValidOptionCode(int rawEnumValue, int rawLastEnumValue) const
{
    return Menu::isValidOptionCode(rawEnumValue, rawLastEnumValue);
}