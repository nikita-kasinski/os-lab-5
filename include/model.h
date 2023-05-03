//
// Created by Nikita Kasinski
//

#pragma once
#include <map>
#include <cstdlib>
#include <string>
#include <fstream>
#include <vector>
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
    void writeBinaryFile(const std::vector<Employee> &employees) const;

    // retrieves record by recordId
    Employee readRecord(const std::size_t &recordId) const;

    // rewrites record via recordId
    void writeRecord(const std::size_t &recordId, const Employee& newRecord) const;
};