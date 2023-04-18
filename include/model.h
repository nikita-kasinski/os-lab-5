#pragma once

//
// Created by Nikita Kasinski
//

#include <map>
#include <cstdlib>
#include <string>
#include <fstream>
#include "employee.h"

class Model
{
private:
    static constexpr std::size_t overallOffset = 0; // sizeof(std::size_t);
    static constexpr std::size_t singleOffset = sizeof(Employee);
    
    std::string binaryFileName;

public:
    // contructs model which will operate binaryFileName file
    Model(const std::string &binaryFileName);

    // fills binary file binaryFileName with records of employess
    void writeBinaryFile(const Employee *employees, const std::size_t &size) const;

    // retrieves record by recordId, returns false if such employee doesn't exist
    Employee readRecord(const std::size_t &recordId) const;
};