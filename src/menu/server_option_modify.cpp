//
// Created by Nikita Kasinski
//

#include "menu/server_option_modify.h"
#include "protocol.h"
#include "utility.h"

ServerMenu::ServerOptionModify::ServerOptionModify(const Menu *menu) : MenuOption(menu)
{
}

ResultCode ServerMenu::ServerOptionModify::execute()
{
    const ServerMenu *serverMenu = dynamic_cast<const ServerMenu *>(_menu);
    auto pipe = serverMenu->_pipe;
    HANDLE pipeUnwrapped = SmartWinapi::unwrap(pipe);
    auto ctrl = serverMenu->_ctrl;
    auto threadId = serverMenu->_threadId;
    auto writer = serverMenu->_writer;

    DWORD bytes;

    // reading key
    int key;
    ReadFile(pipeUnwrapped, reinterpret_cast<char *>(&key), sizeof(int), &bytes, NULL);

    // trying to get recordId by id
    size_t recordId;
    bool keyExists = ctrl->idToRecordId(key, recordId);

    if (keyExists)
    {
        // record exists

        HANDLE recordAccess = OpenMutexA(MUTEX_ALL_ACCESS, FALSE, Utility::getAccessMutexName(recordId).c_str());

        writer->write("Thread ", threadId, " is waiting for record ", recordId, " to become available for modifying\n");

        // waiting for record access
        WaitForSingleObject(recordAccess, INFINITE);

        writer->write("Thread ", threadId, " access granted\n");

        // writing success response
        WriteFile(pipeUnwrapped, &Protocol::SUCCESS, Protocol::SIZE, &bytes, NULL);

        // reading record
        Employee employeeRead;
        if (ctrl->getRecord(key, employeeRead))
        {

            // writing record
            WriteFile(pipeUnwrapped, reinterpret_cast<char *>(&employeeRead), sizeof(Employee), &bytes, NULL);

            // reading new record
            ReadFile(pipeUnwrapped, reinterpret_cast<char *>(&employeeRead), sizeof(Employee), &bytes, NULL);

            if (ctrl->setRecord(key, employeeRead))
            {
                WriteFile(pipeUnwrapped, &Protocol::SUCCESS, Protocol::SIZE, &bytes, NULL);

                char response;
                ReadFile(pipeUnwrapped, &response, Protocol::SIZE, &bytes, NULL);

                if (Protocol::FINISH != response)
                {
                    writer->write("Thread ", threadId, " protocol violation. Abort\n");
                    return ResultCode::ProtocolViolation;
                }
            }
            else
            {
                WriteFile(pipeUnwrapped, &Protocol::FAILURE, Protocol::SIZE, &bytes, NULL);
            }
        }
        else
        {
            WriteFile(pipeUnwrapped, &Protocol::FAILURE, Protocol::SIZE, &bytes, NULL);
        }

        // marking record as not being modified
        ReleaseMutex(recordAccess);
    }
    else
    {
        // writing failure response
        WriteFile(pipeUnwrapped, &Protocol::FAILURE, Protocol::SIZE, &bytes, NULL);
    }
    return ResultCode::OK;
}

bool ServerMenu::ServerOptionModify::isQuitOption() const
{
    return false;
}