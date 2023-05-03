//
// Created by Nikita Kasinski
//
#pragma once

#include <windows.h>
#include <memory>

class SmartWinapi
{
private:
    struct CriticalSectionDeleter
    {
        void operator()(CRITICAL_SECTION *cs) const;
    };

    struct HandleCloser
    {
        void operator()(HANDLE *handle) const;
    };

    static CRITICAL_SECTION *createCriticalSection();
    static HANDLE *createHandle(HANDLE source);

public:
    static std::unique_ptr<CRITICAL_SECTION, CriticalSectionDeleter> make_unique_cs();
    static std::unique_ptr<HANDLE, HandleCloser> make_unique_handle(HANDLE source);

    static std::shared_ptr<CRITICAL_SECTION> make_shared_cs();
    static std::shared_ptr<HANDLE> make_shared_handle(HANDLE source);
};