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
        size_t request = Utility::safeUnsignedIntegerInput(std::cin, std::cout, "Enter request: ", "Value must be positive integer\n");
        DWORD bytes;

        if (request == 0)
        {
            std::cout << menu;
        }
        else if (request == 1)
        {
            WriteFile(SmartWinapi::unwrap(pipe), &Protocol::QUIT, Protocol::SIZE, &bytes, NULL);

            std::cout << "Quit.\n";
            break;
        }
        else if (request == 2)
        {
            // reading record

            int key = -1;
            std::cout << keyPrompt;
            std::cin >> key;

            WriteFile(SmartWinapi::unwrap(pipe), &Protocol::READ, Protocol::SIZE, &bytes, NULL);
            WriteFile(SmartWinapi::unwrap(pipe), reinterpret_cast<char*>(&key), sizeof(int), &bytes, NULL);

            std::cout << "Waiting for record to become available for reading\n";

            char response;
            ReadFile(SmartWinapi::unwrap(pipe), &response, Protocol::SIZE, &bytes, NULL);

            
            if (Protocol::FAILURE == response)
            {
                std::cout << "There is no employee under such id\n";
                continue;
            }
            else if (Protocol::SUCCESS == response)
            {
                std::cout << "Access granted\n";

                Employee employeeRead;
                ReadFile(SmartWinapi::unwrap(pipe), reinterpret_cast<char*>(&employeeRead), sizeof(Employee), &bytes, NULL);
                std::cout << "Employee received\n";
                Utility::printEmployee(std::cout, employeeRead);
                std::cout << "Enter any key to continue\n";
                char x;
                std::cin >> x;
                WriteFile(SmartWinapi::unwrap(pipe), &Protocol::FINISH, Protocol::SIZE, &bytes, NULL);
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

            WriteFile(SmartWinapi::unwrap(pipe), &Protocol::MODIFY, Protocol::SIZE, &bytes, NULL);
            WriteFile(SmartWinapi::unwrap(pipe), reinterpret_cast<char*>(&key), sizeof(int), &bytes, NULL);

            std::cout << "Waiting for record to become available for modifying\n";

            char response;
            ReadFile(SmartWinapi::unwrap(pipe), &response, Protocol::SIZE, &bytes, NULL);


            if (Protocol::SUCCESS == response)
            {
                std::cout << "Access granted\n";

                // reading old record
                Employee employeeRead;
                ReadFile(SmartWinapi::unwrap(pipe), reinterpret_cast<char*>(&employeeRead), sizeof(Employee), &bytes, NULL);

                // printing old record
                std::cout << "Old record\n";
                Utility::printEmployee(std::cout, employeeRead);

                // recieving new record
                std::cout << "Enter new values for this record\n";
                employeeRead = Utility::readEmployee(std::cin, std::cout);

                // writing rew record to pipe
                WriteFile(SmartWinapi::unwrap(pipe), reinterpret_cast<char*>(&employeeRead), sizeof(Employee), &bytes, NULL);

                // reading response
                char response;
                ReadFile(SmartWinapi::unwrap(pipe), &response, Protocol::SIZE, &bytes, NULL);

                if (Protocol::SUCCESS == response)
                {
                    std::cout << "The record was changed successfully\n";
                    std::cout << "Enter any key to continue\n";
                    char x;
                    std::cin >> x;
                    WriteFile(SmartWinapi::unwrap(pipe), &Protocol::FINISH, Protocol::SIZE, &bytes, NULL);
                }
                else if (Protocol::FAILURE == response)
                {
                    std::cout << "The id of new record is already taken\n";
                    continue;
                }   
                else
                {
                    std::cout << "Protocol violation. Abort\n";
                    break;
                }
            }
            else if (Protocol::FAILURE == response)
            {
                std::cout << "No record under such id\n";
                continue;
            }
            else
            {
                std::cout << "Protocol violation. Abort\n";
                break;
            }
        }
        else
        {
            std::cout << "Unknown option. Enter 0 to see menu\n";
        }
    }
    return 0;
}