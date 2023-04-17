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
    static constexpr size_t overallOffset = sizeof(size_t);
    static constexpr size_t singleOffset = sizeof(Employee);

public:

    // initializes binary files with given array of employees
    static void initBinaryFile(const std::string &binaryFileName, const Employee *employees, const size_t &size);

    // gets record by given id and puts it into employee, return false in case there is no employee under such id
    static bool getRecord(std::istream& f, const size_t& id, Employee& employee);
    
};