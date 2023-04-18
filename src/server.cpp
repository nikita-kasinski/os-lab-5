//
// Created by Nikita Kasinski
//

#include <iostream>
#include <windows.h>
#include <sstream>
#include "utility.h"
#include "controller.h"

BOOL StartClient()
{
    STARTUPINFOA si;
    ZeroMemory(&si, sizeof(STARTUPINFOA));
    si.cb = sizeof(STARTUPINFOA);

    PROCESS_INFORMATION pi;

    std::ostringstream consoleCommand;
    std::string client = "client.exe";
    consoleCommand << client;
    BOOL result = CreateProcessA(
        NULL,
        const_cast<char *>(consoleCommand.str().c_str()),
        NULL,
        NULL,
        FALSE,
        CREATE_NEW_CONSOLE,
        NULL,
        NULL,
        &si,
        &pi);
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    return result;
}

DWORD WINAPI InteractWithClientThread(LPVOID args)
{
    const std::string pipeName = R"(\\.\pipe\os-lab5-pipe)";
    size_t numberOfClients = *(size_t *)args;

    // creating named pipe
    HANDLE pipe = CreateNamedPipeA(
        pipeName.c_str(),
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        numberOfClients,
        0,
        0,
        INFINITE,
        NULL);

    if (pipe == INVALID_HANDLE_VALUE)
    {
        // pipe didn't open
        std::cerr << "Cannot open pipe: " << GetLastError() << "\n";
        return 2;
    }

    if (!ConnectNamedPipe(pipe, NULL))
    {
        CloseHandle(pipe);
        std::cerr << "Connection error: " << GetLastError() << "\n";
        return 3;
    }

    while (true)
    {
        DWORD bytesRead;
        char request;
        std::cout << "Thread is working\n";
        ReadFile(pipe, &request, sizeof(char), &bytesRead, NULL);

        // 0 is quit
        // 1 is read record
        // 2 is modify record

        if (request == 0)
        {
            break;
        }
        else if (request == 1)
        {
            // reading record
        }
        else if (request == 2)
        {
            // modifying record
        }
        else 
        {
            std::cerr << "Protocol violation\n";
        }
    }

    std::cout << "Thread ended\n";
    CloseHandle(pipe);
}

int main()
{
    // retrieving number of employees
    size_t employeesSize = Utility::safeUnsignedIntegerInput(
        std::cin,
        std::cout,
        "Enter number of employees: ",
        "Value must be positive integer\n");

    // retrieving employees
    Employee *employees = new Employee[employeesSize];
    for (size_t i = 0; i < employeesSize; ++i)
    {
        employees[i] = Utility::readEmployee(std::cin, std::cout);
    }

    // retrieving binary file name
    std::string binaryFileName;
    std::cout << "Enter binary file name: ";
    std::cin >> binaryFileName;

    // creating controller for binary file
    bool ok = false;
    Controller ctrl(binaryFileName, employees, employeesSize, ok);
    if (!ok)
    {
        std::cerr << "Employees array has equal ids. Quit\n";
        return 1;
    }

    // printing binary file
    {
        size_t employeesFromFileSize;
        Employee *employeesFromFile = ctrl.getAllRecords(employeesFromFileSize);
        Utility::printEmployees(std::cout, employeesFromFile, employeesFromFileSize);
        delete[] employeesFromFile;
    }

    // retrieving number of clients
    size_t numberOfClients = Utility::safeUnsignedIntegerInput(
        std::cin,
        std::cout,
        "Enter number of clients: ",
        "Value must be positive integer\n");

    HANDLE *threads = new HANDLE[numberOfClients];
    for (size_t i = 0; i < numberOfClients; ++i)
    {
        DWORD thread_id;
        threads[i] = CreateThread(NULL, 0, InteractWithClientThread, (LPVOID *)(&numberOfClients), NULL, &thread_id);
    }

    // starting clients
    for (size_t i = 0; i < numberOfClients; ++i)
    {
        StartClient();
    }

    // waiting for all threads to exit. It is better to change INFINITE to some constant like 10 minutes
    WaitForMultipleObjects(numberOfClients, threads, TRUE, INFINITE);

    // freeing memory
    for (size_t i = 0; i < numberOfClients; ++i)
    {
        CloseHandle(threads[i]);
    }
    delete[] threads;
    delete[] employees;
    return 0;
}