#include "args.h"

ClientHandlerArgs::ClientHandlerArgs(
    std::size_t id,
    std::size_t numberOfClients,
    std::size_t numberOfRecords,
    const std::shared_ptr<Controller> &ctrl,
    const std::shared_ptr<CRITICAL_SECTION> &iocs)
{
    _id = id;
    _numberOfClients = numberOfClients;
    _numberOfRecords = numberOfRecords;
    _ctrl = ctrl;
    _iocs = iocs;
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

std::shared_ptr<CRITICAL_SECTION> ClientHandlerArgs::getCriticalSection() const
{
    return _iocs;
}