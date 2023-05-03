//
// Created by Nikita Kasinski
//

#include "smart_winapi.h"

void CriticalSectionDeleter::operator()(CRITICAL_SECTION *cs) const
{
    DeleteCriticalSection(cs);
    delete cs;
}

void HandleCloser::operator()(HANDLE *handle) const
{
    CloseHandle(handle);
    delete handle;
}

std::unique_ptr<CRITICAL_SECTION, CriticalSectionDeleter> make_unique_cs()
{
    CRITICAL_SECTION *csPointer = new CRITICAL_SECTION;
    InitializeCriticalSection(csPointer);
    return std::unique_ptr<CRITICAL_SECTION, CriticalSectionDeleter>(csPointer, CriticalSectionDeleter());
}

std::unique_ptr<HANDLE, HandleCloser> make_unique_handle(HANDLE source)
{
    HANDLE *handlePointer = new HANDLE;
    *handlePointer = source;
    return std::unique_ptr<HANDLE, HandleCloser>(handlePointer, HandleCloser());
}