//
// Created by Nikita Kasinski
//

#include <string>
#include <iostream>
#include "clientHandler.h"
#include "args.h"
#include "controller.h"
#include "utility.h"
#include "protocol.h"

DWORD WINAPI ClientHandler(LPVOID _args)
{
    const std::string pipeName = R"(\\.\pipe\os-lab5-pipe)";

    // processing passed arguments
    ClientHandlerArgs args = *(ClientHandlerArgs *)_args;
    std::size_t threadId = args.getId();
    std::size_t numberOfClients = args.getNumberOfClients();
    std::shared_ptr<Controller> ctrl = args.getController();
    std::shared_ptr<CRITICAL_SECTION> iocs = args.getCriticalSection();

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
        EnterCriticalSection(iocs.get());
        std::cerr << "Thread " << threadId << " cannot open a pipe: " << GetLastError() << "\n";
        LeaveCriticalSection(iocs.get());
        return 2;
    }

    if (!ConnectNamedPipe(pipe, NULL))
    {
        CloseHandle(pipe);
        EnterCriticalSection(iocs.get());
        std::cerr << "Thread " << threadId << ". Connection error: " << GetLastError() << "\n";
        LeaveCriticalSection(iocs.get());
        return 3;
    }

    while (true)
    {
        DWORD bytes;
        char request;

        EnterCriticalSection(iocs.get());
        std::cout << "Thread " << threadId << " is working\n";
        LeaveCriticalSection(iocs.get());

        ReadFile(pipe, &request, sizeof(char), &bytes, NULL);

        if (Protocol::QUIT == request)
        {
            DisconnectNamedPipe(pipe);
            break;
        }
        else if (Protocol::READ == request)
        {
            // reading record

            int key; // employee id
            ReadFile(pipe, reinterpret_cast<char *>(&key), sizeof(int), &bytes, NULL);

            EnterCriticalSection(iocs.get());
            std::cout << "Thread " << threadId << " received key to read: " << key << "\n";
            LeaveCriticalSection(iocs.get());

            size_t recordId;
            bool keyExists = ctrl->idToRecordId(key, recordId); // record id

            if (keyExists)
            {
                HANDLE notBeingModifiedEvent = OpenEventA(EVENT_ALL_ACCESS, FALSE, Utility::getWriteEventName(recordId).c_str());
                HANDLE notBeingReadByClientEvent = OpenEventA(EVENT_ALL_ACCESS, FALSE, Utility::getReadEventName(recordId, threadId).c_str());

                EnterCriticalSection(iocs.get());
                std::cout << "Thread " << threadId << " is waiting for record with key " << key << " to become available\n";
                LeaveCriticalSection(iocs.get());

                WaitForSingleObject(notBeingModifiedEvent, INFINITE); // waiting for record to become available

                EnterCriticalSection(iocs.get());
                std::cout << "Thread " << threadId << " access granted\n";
                LeaveCriticalSection(iocs.get());

                // marking record as being read
                ResetEvent(notBeingReadByClientEvent);

                // reading record and sending to client
                Employee employeeRead;
                if (ctrl->getRecord(key, employeeRead))
                {

                    EnterCriticalSection(iocs.get());
                    std::cout << "Thread " << threadId << " has read employee and has sent it\n";
                    LeaveCriticalSection(iocs.get());

                    WriteFile(pipe, &Protocol::SUCCESS, Protocol::SIZE, &bytes, NULL);
                    WriteFile(pipe, reinterpret_cast<char *>(&employeeRead), sizeof(Employee), &bytes, NULL);

                    char response;

                    // waiting for client to finish access to record
                    EnterCriticalSection(iocs.get());
                    std::cout << "Thread " << threadId << " is waiting for client to stop reading record\n";
                    LeaveCriticalSection(iocs.get());

                    ReadFile(pipe, &response, Protocol::SIZE, &bytes, NULL);

                    if (Protocol::FINISH != response)
                    {
                        EnterCriticalSection(iocs.get());
                        std::cerr << "Protocol violation on stopping reading a record\n. Exit thread";
                        LeaveCriticalSection(iocs.get());
                        ExitThread(2);
                    }

                    EnterCriticalSection(iocs.get());
                    std::cout << "Thread " << threadId << " has stopped reading record\n";
                    LeaveCriticalSection(iocs.get());
                }
                else
                {
                    WriteFile(pipe, &Protocol::FAILURE, Protocol::SIZE, &bytes, NULL);
                }

                // marking record as not being read
                SetEvent(notBeingReadByClientEvent);
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

            // reading key
            int key;
            ReadFile(pipe, reinterpret_cast<char *>(&key), sizeof(int), &bytes, NULL);

            // trying to get recordId by id
            size_t recordId;
            bool keyExists = ctrl->idToRecordId(key, recordId);

            if (keyExists)
            {
                // record exists

                // opening events
                HANDLE notBeingModifiedEvent = OpenEventA(EVENT_ALL_ACCESS, FALSE, Utility::getWriteEventName(recordId).c_str());
                HANDLE *notBeingReadEvents = new HANDLE[numberOfClients];
                for (size_t i = 0; i < numberOfClients; ++i)
                {
                    notBeingReadEvents[i] = OpenEventA(EVENT_ALL_ACCESS, FALSE, Utility::getReadEventName(recordId, i).c_str());
                }

                EnterCriticalSection(iocs.get());
                std::cout << "Thread " << threadId << " is waiting for record " << recordId << " to become available for modifying\n";
                LeaveCriticalSection(iocs.get());

                // waiting for all clients to stop reading record
                WaitForMultipleObjects(numberOfClients, notBeingReadEvents, TRUE, INFINITE);
                WaitForSingleObject(notBeingModifiedEvent, INFINITE);

                // marking record as being modified
                ResetEvent(notBeingModifiedEvent);

                EnterCriticalSection(iocs.get());
                std::cout << "Thread " << threadId << " access granted\n";
                LeaveCriticalSection(iocs.get());

                // writing success response
                WriteFile(pipe, &Protocol::SUCCESS, Protocol::SIZE, &bytes, NULL);

                // reading record
                Employee employeeRead;
                if (ctrl->getRecord(key, employeeRead))
                {

                    // writing record
                    WriteFile(pipe, reinterpret_cast<char *>(&employeeRead), sizeof(Employee), &bytes, NULL);

                    // reading new record
                    ReadFile(pipe, reinterpret_cast<char *>(&employeeRead), sizeof(Employee), &bytes, NULL);

                    if (ctrl->setRecord(key, employeeRead))
                    {
                        WriteFile(pipe, &Protocol::SUCCESS, Protocol::SIZE, &bytes, NULL);

                        char response;
                        ReadFile(pipe, &response, Protocol::SIZE, &bytes, NULL);

                        if (Protocol::FINISH != response)
                        {
                            EnterCriticalSection(iocs.get());
                            std::cout << "Thread " << threadId << " protocol violation. Abort\n";
                            LeaveCriticalSection(iocs.get());
                            ExitThread(4);
                        }
                    }
                    else
                    {
                        WriteFile(pipe, &Protocol::FAILURE, Protocol::SIZE, &bytes, NULL);
                    }
                }
                else
                {
                    WriteFile(pipe, &Protocol::FAILURE, Protocol::SIZE, &bytes, NULL);
                }

                // marking record as not being modified
                SetEvent(notBeingModifiedEvent);
                CloseHandle(notBeingModifiedEvent);
                for (size_t i = 0; i < numberOfClients; ++i)
                {
                    CloseHandle(notBeingReadEvents[i]);
                }
                delete[] notBeingReadEvents;
            }
            else
            {
                // writing failure response
                WriteFile(pipe, &Protocol::FAILURE, Protocol::SIZE, &bytes, NULL);
            }
        }
        else
        {
            EnterCriticalSection(iocs.get());
            std::cerr << "Protocol violation. Exit thread\n";
            LeaveCriticalSection(iocs.get());

            ExitThread(3);
        }
    }

    EnterCriticalSection(iocs.get());
    std::cout << "Thread " << threadId << " ended\n";
    LeaveCriticalSection(iocs.get());

    CloseHandle(pipe);

    return 0;
}