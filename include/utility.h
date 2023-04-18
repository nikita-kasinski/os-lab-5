#pragma once
//
// Created by Nikita Kasinski
//

#include <limits>
#include <fstream>
#include <map>
#include "employee.h"

class Utility
{
private:
    static constexpr auto maxStreamSize = std::numeric_limits<std::streamsize>::max();

public:
    // checks if the array has equal elements
    static bool hasEquals(const int *array, const size_t &size);

    // creates array of ids of given employees, returned array must be freed
    static int *extractIds(const Employee *employees, const size_t &size);
};