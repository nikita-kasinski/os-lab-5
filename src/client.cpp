//
// Created by Nikita Kasinski
//

#include <iostream>
#include <string>
#include <windows.h>
#include "utility.h"

int main()
{
    const std::string pipeName = R"(\\.\pipe\os-lab5-pipe)";

    // creating pipe
    HANDLE pipe = CreateFileA(
        pipeName.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);

    if (pipe == INVALID_HANDLE_VALUE)
    {
        std::cerr << "Connection with the named pipe failed: " << GetLastError() << "\n";
        return 2;
    }

    std::string menu =
        "Enter corresponding request: \n"
        "   0 to see menu\n"
        "   1 to quit\n"
        "   2 to read record\n"
        "   3 to modify record\n";
    std::string keyPrompt = "Input employee id to access its record: ";

    std::cout << menu;
    while (true)
    {
        int request = -1;
        std::cin >> request;
        if (request == 0)
        {
            std::cout << menu;
        }
        else if (request == 1)
        {
            char response = 0;
            DWORD bytesWritten;
            WriteFile(pipe, &response, sizeof(response), &bytesWritten, NULL);
            std::cout << "Quit.\n";
            break;
        }
        else if (request == 2)
        {
            // reading record

            int key = -1;
            std::cout << keyPrompt;
            std::cin >> key;
        }
        else if (request == 3)
        {
            // modifying record

            int key = -1;
            std::cout << keyPrompt;
            std::cin >> key;
        }
        else
        {
            std::cout << "Unknown option. Enter 0 to see menu\n";
        }
    }
    return 0;
}