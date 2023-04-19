//
// Created by Nikita Kasinski
//

#include <iostream>
#include <string>
#include <windows.h>
#include "utility.h"
#include "protocol.h"

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
        DWORD bytes;

        if (request == 0)
        {
            std::cout << menu;
        }
        else if (request == 1)
        {
            WriteFile(pipe, &Protocol::QUIT, Protocol::SIZE, &bytes, NULL);

            std::cout << "Quit.\n";
            break;
        }
        else if (request == 2)
        {
            // reading record

            int key = -1;
            std::cout << keyPrompt;
            std::cin >> key;

            WriteFile(pipe, &Protocol::READ, Protocol::SIZE, &bytes, NULL);
            WriteFile(pipe, reinterpret_cast<char*>(&key), sizeof(int), &bytes, NULL);

            char response;
            ReadFile(pipe, &response, 1, &bytes, NULL);
            
            if (Protocol::FAILURE == response)
            {
                std::cout << "There is no employee under such id\n";
                continue;
            }
            else if (Protocol::SUCCESS == response)
            {
                Employee employeeRead;
                ReadFile(pipe, reinterpret_cast<char*>(&employeeRead), sizeof(Employee), &bytes, NULL);
                std::cout << "Employee received\n";
                Utility::printEmployee(std::cout, employeeRead);
                std::cout << "Enter any key to continue\n";
                char x;
                std::cin >> x;
                WriteFile(pipe, &Protocol::FINISH, Protocol::SIZE, &bytes, NULL);
            }
            else 
            {
                std::cerr << "Protocol violation. Abort\n";
                break;
            }
        }
        else if (request == 3)
        {
            // modifying record

            int key = -1;
            std::cout << keyPrompt;
            std::cin >> key;

            WriteFile(pipe, &Protocol::MODIFY, Protocol::SIZE, &bytes, NULL);
            WriteFile(pipe, reinterpret_cast<char*>(&key), sizeof(int), &bytes, NULL);
        }
        else
        {
            std::cout << "Unknown option. Enter 0 to see menu\n";
        }
    }
    return 0;
}