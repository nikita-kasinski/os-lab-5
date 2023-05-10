//
// Created by Nikita Kasinski
//

#include "menu/server_option_read.h"
#include "protocol.h"
#include "utility.h"

ServerMenu::ServerOptionRead::ServerOptionRead(const Menu *menu) : MenuOption(menu)
{
}

ResultCode ServerMenu::ServerOptionRead::execute()
{
    const ServerMenu *serverMenu = dynamic_cast<const ServerMenu *>(_menu);
    HANDLE unwrappedPipe = SmartWinapi::unwrap(serverMenu->_pipe);
    auto writer = serverMenu->_writer;
    auto threadId = serverMenu->_threadId;
    auto ctrl = serverMenu->_ctrl;
    auto recordAccessReadCount = serverMenu->_recordAccessReadCount;
    auto acs = serverMenu->_acs;

    // reading record
    DWORD bytes;

    int key; // employee id
    ReadFile(unwrappedPipe, reinterpret_cast<char *>(&key), sizeof(int), &bytes, NULL);

    writer->write("Thread ", threadId, " received key to read: ", key, "\n");

    size_t recordId;
    bool keyExists = ctrl->idToRecordId(key, recordId); // record id

    if (keyExists)
    {
        HANDLE recordAccess = OpenMutexA(MUTEX_ALL_ACCESS, FALSE, Utility::getAccessMutexName(recordId).c_str());

        writer->write("Thread ", threadId, " is waiting for record with key ", key, " to become available\n");

        EnterCriticalSection(acs.get());
        if (recordAccessReadCount->at(recordId) == 0)
        {
            // record is not being read by any client. Waiting for access
            WaitForSingleObject(recordAccess, INFINITE); // waiting for record to become available
        }
        ++recordAccessReadCount->at(recordId);
        LeaveCriticalSection(acs.get());

        writer->write("Thread ", threadId, " access granted\n");

        // reading record and sending to client
        Employee employeeRead;
        if (ctrl->getRecord(key, employeeRead))
        {

            writer->write("Thread ", threadId, " has read employee and has sent it\n");

            WriteFile(unwrappedPipe, &Protocol::SUCCESS, Protocol::SIZE, &bytes, NULL);
            WriteFile(unwrappedPipe, reinterpret_cast<char *>(&employeeRead), sizeof(Employee), &bytes, NULL);

            char response;

            // waiting for client to finish access to record
            writer->write("Thread ", threadId, " is waiting for client to stop reading record\n");

            ReadFile(unwrappedPipe, &response, Protocol::SIZE, &bytes, NULL);

            if (Protocol::FINISH != response)
            {
                writer->write("Protocol violation on stopping reading a record\n. Exit thread");
                return ResultCode::ProtocolViolation;
            }

            writer->write("Thread ", threadId, " has stopped reading record\n");
        }
        else
        {
            WriteFile(unwrappedPipe, &Protocol::FAILURE, Protocol::SIZE, &bytes, NULL);
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
        WriteFile(unwrappedPipe, &Protocol::FAILURE, Protocol::SIZE, &bytes, NULL);
    }
    return ResultCode::OK;
}

bool ServerMenu::ServerOptionRead::isQuitOption() const
{
    return false;
}