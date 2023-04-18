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
    static constexpr size_t overallOffset = 0; // sizeof(size_t);
    static constexpr size_t singleOffset = sizeof(Employee);
    std::map<int, size_t> idToRecordId;
    std::string binaryFileName;

public:
    // contructs model which will operate binaryFileName file
    Model(const std::string &binaryFileName);

    // fills idToRecordId map. Employee.id -> position in the binary file
    void fillMap(const Employee *employees, const size_t &size);

    // fills binary file binaryFileName with records of employess
    void writeBinaryFile(const Employee *employees, const size_t &size) const;

    // retrieves record by employee id, returns false if such employee doesn't exist
    bool getRecord(const size_t &id, Employee &employee) const;
};