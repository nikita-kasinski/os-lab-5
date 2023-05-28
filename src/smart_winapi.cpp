//
// Created by Nikita Kasinski
//

#include <iostream>
#include "smart_winapi.h"

void SmartWinapi::CriticalSectionDeleter::operator()(CRITICAL_SECTION *cs) const
{
    DeleteCriticalSection(cs);
    std::cout << "Smart critical section was destructed\n";
    delete cs;
}

void SmartWinapi::HandleCloser::operator()(HANDLE *handle) const
{
    CloseHandle(handle);
    std::cout << "Smart handle was destructed\n";
    delete handle;
}

void SmartWinapi::MutexGuard::operator()(HANDLE *handle) const
{
    ReleaseMutex(handle);
    std::cout << "Mutex released via Mutex Guard\n";
    delete handle;
}

CRITICAL_SECTION* SmartWinapi::createCriticalSection()
{
    CRITICAL_SECTION *csPointer = new CRITICAL_SECTION;
    InitializeCriticalSection(csPointer);
    std::cout << "Smart critical section was initialized\n";
    return csPointer;
}

HANDLE *SmartWinapi::createHandle(HANDLE source)
{
    HANDLE *handlePointer = new HANDLE;
    *handlePointer = source;
    std::cout << "Smart handle was created\n";
    return handlePointer;
}

std::unique_ptr<CRITICAL_SECTION, SmartWinapi::CriticalSectionDeleter> SmartWinapi::make_unique_cs()
{
    return std::unique_ptr<CRITICAL_SECTION, CriticalSectionDeleter>(createCriticalSection(), CriticalSectionDeleter());
}

std::unique_ptr<HANDLE, SmartWinapi::HandleCloser> SmartWinapi::make_unique_handle(HANDLE source)
{
    return std::unique_ptr<HANDLE, HandleCloser>(createHandle(source), HandleCloser());
}

std::unique_ptr<HANDLE, SmartWinapi::MutexGuard> SmartWinapi::make_unique_mutex(HANDLE mutex)
{
    return std::unique_ptr<HANDLE, MutexGuard>(createHandle(mutex), MutexGuard());
}

std::shared_ptr<CRITICAL_SECTION> SmartWinapi::make_shared_cs()
{
    return std::shared_ptr<CRITICAL_SECTION>(createCriticalSection(), CriticalSectionDeleter());
}

std::shared_ptr<HANDLE> SmartWinapi::make_shared_handle(HANDLE source)
{
    return std::shared_ptr<HANDLE>(createHandle(source), HandleCloser());
}

template <typename SmartPtr>
std::vector<HANDLE> SmartWinapi::unwrapSmartPointersHandleArray(const std::vector<SmartPtr> &array)
{
    std::vector<HANDLE> result(array.size());
    for (std::size_t i = 0; i < array.size(); ++i)
    {
        result[i] = *array[i].get();
    }
    return result;
}

template std::vector<HANDLE> SmartWinapi::unwrapSmartPointersHandleArray(const std::vector<std::unique_ptr<HANDLE, HandleCloser>>&);

template std::vector<HANDLE> SmartWinapi::unwrapSmartPointersHandleArray(const std::vector<std::shared_ptr<HANDLE>>&);

template <typename SmartPtr>
HANDLE SmartWinapi::unwrap(const SmartPtr &source)
{
    return *source.get();
}

template HANDLE SmartWinapi::unwrap(const std::shared_ptr<HANDLE>&);
template HANDLE SmartWinapi::unwrap(const std::unique_ptr<HANDLE, HandleCloser>&);