//
// Created by Nikita Kasinski
//

#pragma once

#include <windows.h>
#include <memory>
#include <ostream>
#include "smart_winapi.h"

class ConcurrentWriter
{
private:
    std::shared_ptr<CRITICAL_SECTION> _iocs;
    bool _locked;
    std::ostream &_out;
    void lock();
    void unlock();

public:
    ConcurrentWriter(const std::shared_ptr<CRITICAL_SECTION> &iocs, std::ostream &out);
    template <typename Current, typename... Following>
    void write(Current current, Following... following)
    {
        lock();
        _out<<(current);
        write(following...);
    }
    template <typename Last>
    void write(Last last)
    {
        lock();
        _out<<(last);
        unlock();
    }
};