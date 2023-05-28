//
// Created by Nikita Kasinski
//

#pragma once

#include <windows.h>
#include <memory>
#include <vector>

#include "result_codes.h"

class SmartWinapi
{
public:
    struct CriticalSectionDeleter
    {
        void operator()(CRITICAL_SECTION *cs) const;
    };

    struct HandleCloser
    {
        void operator()(HANDLE *handle) const;
    };

    struct MutexGuard
    {
        void operator()(HANDLE *handle) const;
    };

    static std::unique_ptr<CRITICAL_SECTION, CriticalSectionDeleter> make_unique_cs();
    static std::unique_ptr<HANDLE, HandleCloser> make_unique_handle(HANDLE source);
    static std::unique_ptr<HANDLE, MutexGuard> make_unique_mutex(HANDLE mutex);

    static std::shared_ptr<CRITICAL_SECTION> make_shared_cs();
    static std::shared_ptr<HANDLE> make_shared_handle(HANDLE source);

    template <class T>
    static ResultCode readPipe(const std::shared_ptr<HANDLE> &pipe, T &result)
    {
        DWORD bytes = 0;
        auto readResult = ReadFile(SmartWinapi::unwrap(pipe), reinterpret_cast<char *>(&result), sizeof(T), &bytes, NULL);
        if (readResult != TRUE)
        {
            return ResultCode::PipeReadError;
        }
        if (bytes != sizeof(T))
        {
            return ResultCode::PipeReadInvalidSize;
        }
        return ResultCode::OK;
    }

    template <class T>
    static ResultCode writePipe(const std::shared_ptr<HANDLE> &pipe, const T &message)
    {
        DWORD bytes;
        auto writeResult = WriteFile(SmartWinapi::unwrap(pipe), reinterpret_cast<const char*>(&message), sizeof(T), &bytes, NULL);
        if (writeResult != TRUE)
        {
            return ResultCode::PipeWriteError;
        }
        if (bytes != sizeof(T))
        {
            return ResultCode::PipeWriteInvalidSize;
        }
        return ResultCode::OK;
    }

    template <typename SmartPtr>
    static std::vector<HANDLE> unwrapSmartPointersHandleArray(const std::vector<SmartPtr> &array);

    template <typename SmartPtr>
    static HANDLE unwrap(const SmartPtr &source);

private:
    static CRITICAL_SECTION *createCriticalSection();
    static HANDLE *createHandle(HANDLE source);
};