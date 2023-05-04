//
// Created by Nikita Kasinski
//

#include "args.h"

ClientHandlerArgs::ClientHandlerArgs()
{
    _id = 0;
    _numberOfClients = 0;
    _numberOfRecords = 0;
    _recordAccessReadCount = std::shared_ptr<std::vector<std::size_t>>();
    _ctrl = std::shared_ptr<Controller>();
    _iocs = std::shared_ptr<CRITICAL_SECTION>();
    _acs = std::shared_ptr<CRITICAL_SECTION>();
}

ClientHandlerArgs::ClientHandlerArgs(
    std::size_t id,
    std::size_t numberOfClients,
    std::size_t numberOfRecords,
    const std::shared_ptr<std::vector<std::size_t>> &recordAccessReadCount,
    const std::shared_ptr<Controller> &ctrl,
    const std::shared_ptr<CRITICAL_SECTION> &iocs,
    const std::shared_ptr<CRITICAL_SECTION> &acs)
{
    _id = id;
    _numberOfClients = numberOfClients;
    _numberOfRecords = numberOfRecords;
    _recordAccessReadCount = recordAccessReadCount;
    _ctrl = ctrl;
    _iocs = iocs;
    _acs = acs;
}

std::size_t ClientHandlerArgs::getId() const
{
    return _id;
}

std::size_t ClientHandlerArgs::getNumberOfClients() const
{
    return _numberOfClients;
}

std::size_t ClientHandlerArgs::getNumberOfRecords() const
{
    return _numberOfRecords;
}

std::shared_ptr<Controller> ClientHandlerArgs::getController() const
{
    return _ctrl;
}

std::shared_ptr<CRITICAL_SECTION> ClientHandlerArgs::getIOCriticalSection() const
{
    return _iocs;
}

std::shared_ptr<CRITICAL_SECTION> ClientHandlerArgs::getArrayCriticalSection() const
{
    return _acs;
}

std::shared_ptr<std::vector<std::size_t>> ClientHandlerArgs::getRecordAccessReadCount() const
{
    return _recordAccessReadCount;
}