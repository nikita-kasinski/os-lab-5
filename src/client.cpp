//
// Created by Nikita Kasinski
//

#include <iostream>
#include <string>
#include <windows.h>
#include "utility.h"
#include "protocol.h"
#include "client_menu.h"
#include "smart_winapi.h"

int main()
{
    const std::string pipeName = R"(\\.\pipe\os-lab5-pipe)";

    // creating pipe
    auto pipe = SmartWinapi::make_shared_handle(CreateFileA(
        pipeName.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        0,
        NULL));

    if (SmartWinapi::unwrap(pipe) == INVALID_HANDLE_VALUE)
    {
        std::cerr << "Connection with the named pipe failed: " << GetLastError() << "\n";
        return 2;
    }

    ClientMenu menu(pipe, std::cout, std::cin);
    int result = menu.start();
    if (ResultCode::OK != result)
    {
        std::cout << result;
    }
    return 0;
}