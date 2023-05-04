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
#include "concurrent_writer.h"
#include "smart_winapi.h"

DWORD WINAPI ClientHandler(LPVOID _args)
{
    const std::string pipeName = R"(\\.\pipe\os-lab5-pipe)";

    // processing passed arguments
    ClientHandlerArgs args = *(ClientHandlerArgs *)_args;
    std::size_t threadId = args.getId();
    std::size_t numberOfClients = args.getNumberOfClients();
    std::shared_ptr<std::vector<std::size_t>> recordAccessReadCount = args.getRecordAccessReadCount();
    std::shared_ptr<Controller> ctrl = args.getController();
    std::shared_ptr<CRITICAL_SECTION> iocs = args.getIOCriticalSection();
    std::shared_ptr<CRITICAL_SECTION> acs = args.getArrayCriticalSection();

    ConcurrentWriter writer(iocs, std::cout);

    // creating named pipe
    std::shared_ptr<HANDLE> pipe = SmartWinapi::make_shared_handle(CreateNamedPipeA(
        pipeName.c_str(),
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        numberOfClients,
        0,
        0,
        INFINITE,
        NULL));

    if (SmartWinapi::unwrap(pipe) == INVALID_HANDLE_VALUE)
    {
        // pipe didn't open
        writer.write("Thread ", threadId, " cannot open a pipe: ", GetLastError(), "\n");
        return 2;
    }

    if (!ConnectNamedPipe(SmartWinapi::unwrap(pipe), NULL))
    {
        writer.write("Thread ", threadId, ". Connection error: ", GetLastError(), "\n");
        return 3;
    }

    while (true)
    {
        DWORD bytes;
        char request;

        writer.write("Thread ", threadId, " is working\n");

        ReadFile(SmartWinapi::unwrap(pipe), &request, sizeof(char), &bytes, NULL);

        if (Protocol::QUIT == request)
        {
            DisconnectNamedPipe(SmartWinapi::unwrap(pipe));
            break;
        }
        else if (Protocol::READ == request)
        {
            // reading record

            int key; // employee id
            ReadFile(SmartWinapi::unwrap(pipe), reinterpret_cast<char *>(&key), sizeof(int), &bytes, NULL);

            writer.write("Thread ", threadId, " received key to read: ", key, "\n");

            size_t recordId;
            bool keyExists = ctrl->idToRecordId(key, recordId); // record id

            if (keyExists)
            {
                HANDLE recordAccess = OpenMutexA(MUTEX_ALL_ACCESS, FALSE, Utility::getAccessMutexName(recordId).c_str());

                writer.write("Thread ", threadId, " is waiting for record with key ", key, " to become available\n");

                EnterCriticalSection(acs.get());
                if (recordAccessReadCount->at(recordId) == 0)
                {
                    // record is not being read by any client. Waiting for access
                    WaitForSingleObject(recordAccess, INFINITE); // waiting for record to become available
                }
                ++recordAccessReadCount->at(recordId);
                LeaveCriticalSection(acs.get());

                writer.write("Thread ", threadId, " access granted\n");

                // reading record and sending to client
                Employee employeeRead;
                if (ctrl->getRecord(key, employeeRead))
                {

                    writer.write("Thread ", threadId, " has read employee and has sent it\n");

                    WriteFile(SmartWinapi::unwrap(pipe), &Protocol::SUCCESS, Protocol::SIZE, &bytes, NULL);
                    WriteFile(SmartWinapi::unwrap(pipe), reinterpret_cast<char *>(&employeeRead), sizeof(Employee), &bytes, NULL);

                    char response;

                    // waiting for client to finish access to record
                    writer.write("Thread ", threadId, " is waiting for client to stop reading record\n");

                    ReadFile(SmartWinapi::unwrap(pipe), &response, Protocol::SIZE, &bytes, NULL);

                    if (Protocol::FINISH != response)
                    {
                        writer.write("Protocol violation on stopping reading a record\n. Exit thread");
                        ExitThread(2);
                    }

                    writer.write("Thread ", threadId, " has stopped reading record\n");
                }
                else
                {
                    WriteFile(SmartWinapi::unwrap(pipe), &Protocol::FAILURE, Protocol::SIZE, &bytes, NULL);
                }

                EnterCriticalSection(acs.get());
                --recordAccessReadCount->at(recordId);
                if (recordAccessReadCount->at(recordId) == 0)
                {
                    // no one is reading the record anymore
                    ReleaseMutex(recordAccess);
                }
                LeaveCriticalSection(acs.get());
            }
            else
            {
                WriteFile(SmartWinapi::unwrap(pipe), &Protocol::FAILURE, Protocol::SIZE, &bytes, NULL);
            }
        }
        else if (Protocol::MODIFY == request)
        {
            // modifying record

            // reading key
            int key;
            ReadFile(SmartWinapi::unwrap(pipe), reinterpret_cast<char *>(&key), sizeof(int), &bytes, NULL);

            // trying to get recordId by id
            size_t recordId;
            bool keyExists = ctrl->idToRecordId(key, recordId);

            if (keyExists)
            {
                // record exists

                HANDLE recordAccess = OpenMutexA(MUTEX_ALL_ACCESS, FALSE, Utility::getAccessMutexName(recordId).c_str());

                writer.write("Thread ", threadId, " is waiting for record ", recordId, " to become available for modifying\n");

                // waiting for record access
                WaitForSingleObject(recordAccess, INFINITE);

                writer.write("Thread ", threadId, " access granted\n");

                // writing success response
                WriteFile(SmartWinapi::unwrap(pipe), &Protocol::SUCCESS, Protocol::SIZE, &bytes, NULL);

                // reading record
                Employee employeeRead;
                if (ctrl->getRecord(key, employeeRead))
                {

                    // writing record
                    WriteFile(SmartWinapi::unwrap(pipe), reinterpret_cast<char *>(&employeeRead), sizeof(Employee), &bytes, NULL);

                    // reading new record
                    ReadFile(SmartWinapi::unwrap(pipe), reinterpret_cast<char *>(&employeeRead), sizeof(Employee), &bytes, NULL);

                    if (ctrl->setRecord(key, employeeRead))
                    {
                        WriteFile(SmartWinapi::unwrap(pipe), &Protocol::SUCCESS, Protocol::SIZE, &bytes, NULL);

                        char response;
                        ReadFile(SmartWinapi::unwrap(pipe), &response, Protocol::SIZE, &bytes, NULL);

                        if (Protocol::FINISH != response)
                        {
                            writer.write("Thread ", threadId, " protocol violation. Abort\n");
                            ExitThread(4);
                        }
                    }
                    else
                    {
                        WriteFile(SmartWinapi::unwrap(pipe), &Protocol::FAILURE, Protocol::SIZE, &bytes, NULL);
                    }
                }
                else
                {
                    WriteFile(SmartWinapi::unwrap(pipe), &Protocol::FAILURE, Protocol::SIZE, &bytes, NULL);
                }

                // marking record as not being modified
                ReleaseMutex(recordAccess);
            }
            else
            {
                // writing failure response
                WriteFile(SmartWinapi::unwrap(pipe), &Protocol::FAILURE, Protocol::SIZE, &bytes, NULL);
            }
        }
        else
        {
            
            writer.write("Protocol violation. Exit thread\n");
            ExitThread(3);
        }
    }

    writer.write("Thread ", threadId, " ended\n");

    return 0;
}