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

    // creating critical section for safe output via stdout
    std::shared_ptr<CRITICAL_SECTION> iocs = std::make_shared<CRITICAL_SECTION>();
    InitializeCriticalSection(iocs.get());

    std::cout << "Initialized io critical section\n";

    std::shared_ptr<std::vector<std::size_t>> recordAccessReadCount = std::make_shared<std::vector<std::size_t>>(numberOfRecords, 0);

    // creating critical section for safe access to recordAccessReadCount array
    std::shared_ptr<CRITICAL_SECTION> acs = std::make_shared<CRITICAL_SECTION>();
    InitializeCriticalSection(acs.get());

    std::cout << "Initialized recordAccessReadCount array\n";

    std::vector<HANDLE> recordAccess(numberOfRecords);
    for (std::size_t i = 0; i < recordAccess.size(); ++i)
    {
        recordAccess[i] = CreateMutexA(NULL, FALSE, Utility::getAccessMutexName(i).c_str());
    }

    // starting client interaction
    std::vector<HANDLE> threads(numberOfClients);
    std::vector<ClientHandlerArgs> args(numberOfClients);
    for (size_t i = 0; i < numberOfClients; ++i)
    {
        DWORD thread_id;
        args[i] = ClientHandlerArgs(i, numberOfClients, numberOfRecords, recordAccessReadCount, ctrl, iocs, acs);
        threads[i] = CreateThread(NULL, 0, ClientHandler, (LPVOID *)(&args[i]), 0, &thread_id);
    }

    std::cout << "Started client handlers\n";
    
    // starting clients
    for (size_t i = 0; i < numberOfClients; ++i)
    {
        StartClient();
    }

    std::cout << "Started clients\n";

    // waiting for all threads to exit. It is better to change INFINITE to some constant like 10 minutes
    WaitForMultipleObjects(numberOfClients, &threads.front(), TRUE, INFINITE);

    std::cout << "Modified binary file\n";
    std::vector<Employee> employeesFromModifiedFile = ctrl->getAllRecords();
    Utility::printEmployees(std::cout, employeesFromModifiedFile);

    // closing handles
    DeleteCriticalSection(iocs.get());
    DeleteCriticalSection(acs.get());
    for (size_t i = 0; i < numberOfClients; ++i)
    {
        CloseHandle(threads[i]);
    }
    return 0;
}