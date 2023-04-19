//
// Created by Nikita Kasinski
//

#include <iostream>
#include <windows.h>
#include <sstream>
#include "utility.h"
#include "controller.h"
#include "args.h"
#include "protocol.h"

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
    std::size_t threadId = args.id;
    std::size_t numberOfClients = args.numberOfClients;
    std::size_t numberOfRecords = args.numberOfRecords;
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
        EnterCriticalSection(iocs);
        std::cerr << "Thread " << threadId << " cannot open a pipe: " << GetLastError() << "\n";
        LeaveCriticalSection(iocs);
        return 2;
    }

    if (!ConnectNamedPipe(pipe, NULL))
    {
        CloseHandle(pipe);
        EnterCriticalSection(iocs);
        std::cerr << "Thread " << threadId << ". Connection error: " << GetLastError() << "\n";
        LeaveCriticalSection(iocs);
        return 3;
    }

    while (true)
    {
        DWORD bytesRead;
        char request;
        EnterCriticalSection(iocs);
        std::cout << "Thread " << threadId << " is working\n";
        LeaveCriticalSection(iocs);

        ReadFile(pipe, &request, sizeof(char), &bytesRead, NULL);

        if (Protocol::QUIT == request)
        {
            break;
        }
        else if (Protocol::READ == request)
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
                HANDLE notBeingModifiedEvent = OpenEventA(EVENT_ALL_ACCESS, FALSE, Utility::getWriteEventName(recordId).c_str());
                HANDLE notBeingReadByClientEvent = OpenEventA(EVENT_ALL_ACCESS, FALSE, Utility::getReadEventName(recordId, threadId).c_str());

                EnterCriticalSection(iocs);
                std::cout << "Thread " << threadId << " is waiting for record with key " << key << " to become available\n";
                LeaveCriticalSection(iocs);

                WaitForSingleObject(notBeingModifiedEvent, INFINITE); // waiting for record to become available

                // marking record as being read
                ResetEvent(notBeingReadByClientEvent);

                // reading record and sending to client
                Employee employeeRead;
                ctrl->getRecord(key, employeeRead);

                EnterCriticalSection(iocs);
                std::cout << "Thread " << threadId << " has read employee and has sent it\n";
                LeaveCriticalSection(iocs);

                WriteFile(pipe, &Protocol::SUCCESS, Protocol::SIZE, &bytes, NULL);
                WriteFile(pipe, reinterpret_cast<char*>(&employeeRead), sizeof(Employee), &bytes, NULL);

                char response;

                // waiting for client to finish access to record
                EnterCriticalSection(iocs);
                std::cout << "Thread " << threadId << " is waiting for client to stop reading record\n";
                LeaveCriticalSection(iocs);

                ReadFile(pipe, &response, Protocol::SIZE, &bytes, NULL);

                if (Protocol::FINISH != response)
                {
                    EnterCriticalSection(iocs);
                    std::cerr << "Protocol violation on stopping reading a record\n. Exit thread";
                    LeaveCriticalSection(iocs);
                    ExitThread(2);
                }

                // marking record as not being read
                SetEvent(notBeingReadByClientEvent);

                EnterCriticalSection(iocs);
                std::cout << "Thread " << threadId << " has stopped reading record\n";
                LeaveCriticalSection(iocs);

                CloseHandle(notBeingModifiedEvent);
                CloseHandle(notBeingReadByClientEvent);
            }
            else
            {
                WriteFile(pipe, &Protocol::FAILURE, Protocol::SIZE, &bytes, NULL);
            }
        }
        else if (Protocol::MODIFY == request)
        {
            // modifying record

        }
        else
        {
            EnterCriticalSection(iocs);
            std::cerr << "Protocol violation. Exit thread\n";
            LeaveCriticalSection(iocs);

            ExitThread(3);
        }
    }

    EnterCriticalSection(iocs);
    std::cout << "Thread ended\n";
    LeaveCriticalSection(iocs);

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
    CRITICAL_SECTION *iocs = new CRITICAL_SECTION;
    InitializeCriticalSection(iocs);

    std::cout << "Initialized critical section\n";

    // creating set of client events for every record
    // event will be set if record i is not read by client j
    HANDLE **notReadEvents = new HANDLE *[numberOfRecords];
    for (size_t i = 0; i < numberOfRecords; ++i)
    {
        notReadEvents[i] = new HANDLE[numberOfClients];
        for (size_t j = 0; j < numberOfClients; ++j)
        {
            notReadEvents[i][j] = CreateEventA(NULL, FALSE, TRUE, Utility::getReadEventName(i, j).c_str());
        }
    }

    std::cout << "Created read events\n";

    // creating write events for every record
    // The event is set when record is not being modified
    HANDLE *notWriteEvents = new HANDLE[numberOfRecords];
    for (size_t i = 0; i < numberOfRecords; ++i)
    {
        notWriteEvents[i] = CreateEventA(NULL, FALSE, TRUE, Utility::getWriteEventName(i).c_str());
    }

    std::cout << "Created write events\n";

    // starting client interaction
    HANDLE *threads = new HANDLE[numberOfClients];
    ThreadArgs *args = new ThreadArgs[numberOfClients];
    for (size_t i = 0; i < numberOfClients; ++i)
    {
        DWORD thread_id;
        args[i].id = i;
        args[i].numberOfClients = numberOfClients;
        args[i].numberOfRecords = numberOfRecords;
        args[i].ctrl = &ctrl;
        args[i].iocs = iocs;
        threads[i] = CreateThread(NULL, 0, InteractWithClientThread, (LPVOID *)(&args[i]), NULL, &thread_id);
    }

    // starting clients
    for (size_t i = 0; i < numberOfClients; ++i)
    {
        StartClient();
    }

    // waiting for all threads to exit. It is better to change INFINITE to some constant like 10 minutes
    WaitForMultipleObjects(numberOfClients, threads, TRUE, INFINITE);

    // freeing memory
    delete[] args;
    DeleteCriticalSection(iocs);
    for (size_t i = 0; i < numberOfRecords; ++i)
    {
        for (size_t j = 0; j < numberOfClients; ++j)
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