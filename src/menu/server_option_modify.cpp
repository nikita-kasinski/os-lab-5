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
    auto ctrl = serverMenu->_ctrl;
    auto threadId = serverMenu->_threadId;
    auto writer = serverMenu->_writer;

    ResultCode writeResult, readResult;

    // reading key
    int key;
    readResult = SmartWinapi::readPipe(pipe, key);
    if (ResultCode::OK != readResult)
    {
        return readResult;
    }

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
        writeResult = SmartWinapi::writePipe(pipe, Protocol::SUCCESS);
        if (ResultCode::OK != writeResult)
        {
            return writeResult;
        }

        // reading record
        Employee employeeRead;
        if (ctrl->getRecord(key, employeeRead))
        {

            // writing record
            writeResult = SmartWinapi::writePipe(pipe, employeeRead);
            if (ResultCode::OK != writeResult)
            {
                return writeResult;
            }

            // reading new record
            readResult = SmartWinapi::readPipe(pipe, employeeRead);
            if (ResultCode::OK != readResult)
            {
                return readResult;
            }

            if (ctrl->setRecord(key, employeeRead))
            {
                writeResult = SmartWinapi::writePipe(pipe, Protocol::SUCCESS);
                if (ResultCode::OK != writeResult)
                {
                    return writeResult;
                }

                Protocol::PROTOCOL response;
                readResult = SmartWinapi::readPipe(pipe, response);
                if (ResultCode::OK != readResult)
                {
                    return readResult;
                }

                if (Protocol::FINISH != response)
                {
                    writer->write("Thread ", threadId, " protocol violation. Abort\n");
                    return ResultCode::ProtocolViolation;
                }
            }
            else
            {
                writeResult = SmartWinapi::writePipe(pipe, Protocol::FAILURE);
                if (ResultCode::OK != writeResult)
                {
                    return writeResult;
                }
            }
        }
        else
        {
            writeResult = SmartWinapi::writePipe(pipe, Protocol::FAILURE);
            if (ResultCode::OK != writeResult)
            {
                return writeResult;
            }
        }

        // marking record as not being modified
        ReleaseMutex(recordAccess);
    }
    else
    {
        // writing failure response
        writeResult = SmartWinapi::writePipe(pipe, Protocol::FAILURE);
    }
    return ResultCode::OK;
}

bool ServerMenu::ServerOptionModify::isQuitOption() const
{
    return false;
}