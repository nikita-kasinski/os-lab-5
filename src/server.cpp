//
// Created by Nikita Kasinski
//

#include <iostream>
#include <windows.h>
#include <sstream>
#include "utility.h"
#include "controller.h"
#include "args.h"

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

DWORD WINAPI InteractWithClientThread(LPVOID _args)
{
    const std::string pipeName = R"(\\.\pipe\os-lab5-pipe)";
    ThreadArgs args = *(ThreadArgs *)_args;
    size_t threadId = args.id;
    size_t numberOfClients = args.numberOfClients;
    size_t numberOfRecords = args.numberOfRecords;
    Controller *ctrl = args.ctrl;
    CRITICAL_SECTION *iocs = args.iocs;

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
            DWORD bytes;

            int key; // employee id
            ReadFile(pipe, reinterpret_cast<char *>(&key), sizeof(int), &bytes, NULL);

            EnterCriticalSection(iocs);
            std::cout << "Thread " << threadId << " received key to read: " << key << "\n";
            LeaveCriticalSection(iocs);

            size_t recordId;
            bool keyExists = ctrl->idToRecordId(key, recordId); // record id

            if (keyExists)
            {
                std::string modifyEventNameTemplate = "Write event ";
                std::ostringstream modifyEventName(modifyEventNameTemplate);
                modifyEventName << recordId;
                HANDLE notBeingModifiedEvent = OpenEventA(EVENT_ALL_ACCESS, FALSE, modifyEventName.str().c_str());

                EnterCriticalSection(iocs);
                std::cout << "Thread " << threadId << "is waiting for record with key " << key << " to become available\n";
                LeaveCriticalSection(iocs);

                WaitForSingleObject(notBeingModifiedEvent, INFINITE); // waiting for record to become available

                char message[5];
                message[0] = 1;
                Sleep(10000);
                memcpy(&message[1], reinterpret_cast<char *>(&key), 4);
                WriteFile(pipe, message, 5, &bytes, NULL);
            }
            else
            {
            }
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

    size_t numberOfRecords = employeesSize;

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

    // creating critical sections for safe output via stdout
    CRITICAL_SECTION iocs;
    InitializeCriticalSection(&iocs);

    // creating set of numberOfClients events for every numberOfRecords
    // event will be set if record i is not read by client j
    HANDLE **notReadEvents = new HANDLE *[numberOfRecords];
    for (size_t i = 0; i < numberOfRecords; ++i)
    {
        notReadEvents[i] = new HANDLE[numberOfClients];
        std::string eventNameTemplate = "Read event";
        for (size_t j = 0; j < numberOfClients; ++i)
        {
            std::ostringstream eventName(eventNameTemplate);
            eventName << i << ' ' << j;
            notReadEvents[i][j] = CreateEventA(NULL, FALSE, TRUE, eventName.str().c_str());
        }
    }

    // creating write events for every record
    // The event is set when record is not being modified
    HANDLE *notWriteEvents = new HANDLE[numberOfRecords];
    for (size_t i = 0; i < numberOfRecords; ++i)
    {
        std::string eventNameTemplate = "Write event ";
        std::ostringstream eventName(eventNameTemplate);
        eventName << i;
        notWriteEvents[i] = CreateEventA(NULL, FALSE, TRUE, eventName.str().c_str());
    }

    // starting client interaction
    HANDLE *threads = new HANDLE[numberOfClients];
    for (size_t i = 0; i < numberOfClients; ++i)
    {
        DWORD thread_id;
        ThreadArgs args;
        args.id = i;
        args.numberOfClients = numberOfClients;
        args.numberOfRecords = numberOfRecords;
        args.ctrl = &ctrl;
        threads[i] = CreateThread(NULL, 0, InteractWithClientThread, (LPVOID *)(&args), NULL, &thread_id);
    }

    // starting clients
    for (size_t i = 0; i < numberOfClients; ++i)
    {
        StartClient();
    }

    // waiting for all threads to exit. It is better to change INFINITE to some constant like 10 minutes
    WaitForMultipleObjects(numberOfClients, threads, TRUE, INFINITE);

    // freeing memory
    DeleteCriticalSection(&iocs);
    for (size_t i = 0; i < numberOfRecords; ++i)
    {
        for (size_t j = 0; j < numberOfClients; ++i)
        {
            CloseHandle(notReadEvents[i][j]);
        }
        delete[] notReadEvents[i];
    }
    delete[] notReadEvents;
    for (size_t i = 0; i < numberOfClients; ++i)
    {
        CloseHandle(threads[i]);
    }
    delete[] threads;
    delete[] employees;
    return 0;
}