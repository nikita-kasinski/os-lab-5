//
// Created by Nikita Kasinski
//

#include <iostream>
#include "concurrent_writer.h"

ConcurrentWriter::ConcurrentWriter(const std::shared_ptr<CRITICAL_SECTION>& iocs, std::ostream &out):
_out(out)
{
    _iocs = iocs;
    _locked = false;
}

void ConcurrentWriter::lock()
{
    if (!_locked)
    {
        EnterCriticalSection(_iocs.get());
        _locked = true;
    }
}

void ConcurrentWriter::unlock()
{
    if (_locked)
    {
        _locked = false;
        LeaveCriticalSection(_iocs.get());
    }
}

