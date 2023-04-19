#pragma once
#include <cstdlib>
#include "controller.h"

struct ThreadArgs
{
    std::size_t id;
    std::size_t numberOfClients;
    std::size_t numberOfRecords;
    Controller *ctrl;
    CRITICAL_SECTION *iocs;
};