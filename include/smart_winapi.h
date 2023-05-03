//
// Created by Nikita Kasinski
//
#pragma once

#include <windows.h>
#include <memory>

struct CriticalSectionDeleter
{
    void operator()(CRITICAL_SECTION *cs) const;
};

struct HandleCloser
{
    void operator()(HANDLE *handle) const;
};

std::unique_ptr<CRITICAL_SECTION, CriticalSectionDeleter> make_unique_cs();
std::unique_ptr<HANDLE, HandleCloser> make_unique_handle(HANDLE source);