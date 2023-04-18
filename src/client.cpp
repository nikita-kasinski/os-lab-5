//
// Created by Nikita Kasinski
//

#include <iostream>
#include <string>
#include "utility.h"

int main()
{
    std::string menu = 
    "Enter corresponding request: \n"
    "   0 to see menu\n"
    "   1 to quit\n"
    "   2 to read record\n"
    "   3 to modify record\n";
    std::string keyPrompt = "Input employee id to access its record: ";

    std::cout << menu;
    while(true)
    {
        int request = -1;
        std::cin >> request;
        if (request == 0)
        {
            std::cout << menu;
        }
        else if (request == 1)
        {
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