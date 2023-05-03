//
// Created by Nikita Kasinski
//

#pragma once
#include <cstdlib>
#include <windows.h>
#include <memory>
#include "controller.h"

// data class for providing thread function ClientHandler with arguments
class ClientHandlerArgs
{
private:
    std::size_t _id;
    std::size_t _numberOfClients;
    std::size_t _numberOfRecords;
    std::shared_ptr<Controller> _ctrl;
    std::shared_ptr<CRITICAL_SECTION> _iocs;
public:
    ClientHandlerArgs(
        std::size_t id, 
        std::size_t numberOfClients, 
        std::size_t numberOfRecords, 
        const std::shared_ptr<Controller>& ctrl, 
        const std::shared_ptr<CRITICAL_SECTION>& iocs);
    std::size_t getId() const;
    std::size_t getNumberOfClients() const;
    std::size_t getNumberOfRecords() const;
    std::shared_ptr<Controller> getController() const;
    std::shared_ptr<CRITICAL_SECTION> getCriticalSection() const;
};