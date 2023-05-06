//
// Created by Nikita Kasinski
//

#include "menu.h"
#include "result_codes.h"

ResultCode Menu::start()
{
    bool started = false;
    while (not started or not option->isQuitOption())
    {
        started = true;

        ResultCode initializeResult = initializeOption();
        if (ResultCode::OK != initializeResult)
            return initializeResult;
        
        ResultCode executeResult = option->execute();
        if (ResultCode::OK != executeResult)
            return executeResult;
    }
    return ResultCode::OK;
}