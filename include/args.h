#pragma once
#include <cstdlib>

struct ThreadArgs
{
    std::size_t id;
    std::size_t numberOfClients;
    std::size_t numberOfRecords;
};