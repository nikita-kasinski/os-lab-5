#pragma once

//
// Created by Nikita Kasinski
//

#include <fstream>
#include <limits>
#include <map>
#include "employee.h"
#include "model.h"

class Controller
{
private:
    Model model;
    std::map<int, std::size_t> _idToRecordId;

public:
    // contructs controller object to manage binary file filled with given array of employees
    Controller(const std::string &binaryFileName, const Employee *employees, const std::size_t &size, bool &ok);

    // gets record by given id and puts it into employee variable, returns false in case there is no employee under such id
    bool getRecord(const int &id, Employee &employee) const;

    // sets record by oldId to employee, returns false in case there is no employee under oldId or newId(employee.id) is taken
    bool setRecord(const int &oldId, const Employee &employee);

    // maps id into recordId. Puts answer to recordId. Returns false if there is no such id
    bool idToRecordId(const int& id, size_t& recordId) const;

    // get all records from a file. Array size is put into size. Array must be freed
    Employee* getAllRecords(size_t& size) const;
};