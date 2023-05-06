//
// Created by Nikita Kasinski
//

#include "menu.h"

int Menu::start()
{
    bool started = false;
    while (not started or not option->isQuitOption())
    {
        started = true;

        int initializeResult = initializeOption();
        if (initializeResult != 0)
            return initializeResult;
        
        int executeResult = option->execute();
        if (executeResult != 0)
            return executeResult;
    }
}