#pragma once
//
// Created by Nikita Kasinski
//

#include <limits>
#include <fstream>
#include "employee.h"

class Utility
{
    static constexpr auto maxStreamSize = std::numeric_limits<std::streamsize>::max();

public:

    // initializes binary files with given array of employees
    static void initBinaryFile(const std::string &binaryFileName, const Employee *employees, const size_t &size);
    
};