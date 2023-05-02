//
// Created by Nikita Kasinski
//

#include <iostream>
#include <windows.h>
#include <sstream>
#include <vector>
#include <memory>
#include "utility.h"
#include "controller.h"
#include "args.h"
#include "protocol.h"
#include "clientHandler.h"

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

int main()
{
    // retrieving number of employees
    size_t employeesSize = Utility::safeUnsignedIntegerInput(
        std::cin,
        std::cout,
        "Enter number of employees: ",
        "Value must be positive integer\n");

    size_t numberOfRecords = employeesSize;

    // retrieving employees
    std::vector<Employee> employees(employeesSize);
    for (size_t i = 0; i < employees.size(); ++i)
    {
        employees[i] = Utility::readEmployee(std::cin, std::cout);
    }
 
    // retrieving binary file name
    std::string binaryFileName;
    std::cout << "Enter binary file name: ";
    std::cin >> binaryFileName;

    // creating controller for binary file
    bool ok = false;
    std::shared_ptr<Controller> ctrl = std::make_shared<Controller>(binaryFileName, employees, ok);
    if (!ok)
    {
        std::cerr << "Employees array has equal ids. Quit\n";
        return 1;
    }

    // printing binary file
    std::vector<Employee> employeesFromUnmodifiedFile = ctrl->getAllRecords();
    Utility::printEmployees(std::cout, employeesFromUnmodifiedFile);

    // retrieving number of clients
    size_t numberOfClients = Utility::safeUnsignedIntegerInput(
        std::cin,
        std::cout,
        "Enter number of clients: ",
        "Value must be positive integer\n");

    // creating critical sections for safe output via stdout
    std::shared_ptr<CRITICAL_SECTION> iocs = std::make_shared<CRITICAL_SECTION>();
    InitializeCriticalSection(iocs.get());

    std::cout << "Initialized critical section\n";

    // creating set of client events for every record
    // event will be set if record i is not read by client j
    std::vector<std::vector<HANDLE>> notReadEvents(numberOfRecords);
    for (size_t i = 0; i < numberOfRecords; ++i)
    {
        notReadEvents[i].resize(numberOfClients);
        for (size_t j = 0; j < numberOfClients; ++j)
        {
            notReadEvents[i][j] = CreateEventA(NULL, TRUE, TRUE, Utility::getReadEventName(i, j).c_str());
        }
    }

    std::cout << "Created read events\n";

    // creating write events for every record
    // The event is set when record is not being modified
    std::vector<HANDLE> notWriteEvents(numberOfRecords);
    for (size_t i = 0; i < notWriteEvents.size(); ++i)
    {
        notWriteEvents[i] = CreateEventA(NULL, TRUE, TRUE, Utility::getWriteEventName(i).c_str());
    }

    std::cout << "Created write events\n";

    // starting client interaction
    std::vector<HANDLE> threads(numberOfClients);
    std::vector<ClientHandlerArgs> args(numberOfClients);
    for (size_t i = 0; i < numberOfClients; ++i)
    {
        DWORD thread_id;
        args[i] = ClientHandlerArgs(i, numberOfClients, numberOfRecords, ctrl, iocs);
        threads[i] = CreateThread(NULL, 0, ClientHandler, (LPVOID *)(&args[i]), 0, &thread_id);
    }

    // starting clients
    for (size_t i = 0; i < numberOfClients; ++i)
    {
        StartClient();
    }

    // waiting for all threads to exit. It is better to change INFINITE to some constant like 10 minutes
    WaitForMultipleObjects(numberOfClients, &threads.front(), TRUE, INFINITE);

    std::cout << "Modified binary file\n";
    std::vector<Employee> employeesFromModifiedFile = ctrl->getAllRecords();
    Utility::printEmployees(std::cout, employeesFromModifiedFile);

    // closing handles
    DeleteCriticalSection(iocs.get());
    for (size_t i = 0; i < numberOfRecords; ++i)
    {
        for (size_t j = 0; j < numberOfClients; ++j)
        {
            CloseHandle(notReadEvents[i][j]);
        }
    }
    for (size_t i = 0; i < numberOfClients; ++i)
    {
        CloseHandle(threads[i]);
    }
    return 0;
}