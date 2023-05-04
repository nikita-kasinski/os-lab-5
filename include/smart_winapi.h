//
// Created by Nikita Kasinski
//
#pragma once

#include <windows.h>
#include <memory>
#include <vector>

class SmartWinapi
{
private:

    static CRITICAL_SECTION *createCriticalSection();
    static HANDLE *createHandle(HANDLE source);

public:
    struct CriticalSectionDeleter
    {
        void operator()(CRITICAL_SECTION *cs) const;
    };

    struct HandleCloser
    {
        void operator()(HANDLE *handle) const;
    };

    static std::unique_ptr<CRITICAL_SECTION, CriticalSectionDeleter> make_unique_cs();
    static std::unique_ptr<HANDLE, HandleCloser> make_unique_handle(HANDLE source);

    static std::shared_ptr<CRITICAL_SECTION> make_shared_cs();
    static std::shared_ptr<HANDLE> make_shared_handle(HANDLE source);

    template <typename SmartPtr>
    static std::vector<HANDLE> unwrapSmartPointersHandleArray(const std::vector<SmartPtr> &array);
    static HANDLE unwrap(const std::shared_ptr<HANDLE> &source);
};