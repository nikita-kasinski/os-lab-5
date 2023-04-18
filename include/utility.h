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
    static constexpr size_t overallOffset = sizeof(size_t);
    static constexpr size_t singleOffset = sizeof(Employee);
    std::map<int, size_t> idToRecordId;
public:

    // contructs utility object to manage binary file filled with given array of employees
    Utility(const std::string &binaryFileName, const Employee *employees, const size_t &size);

    // gets record by given id and puts it into employee, return false in case there is no employee under such id
    bool getRecord(std::istream& f, const size_t& id, Employee& employee);
    
};