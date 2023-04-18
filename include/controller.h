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
    std::map<int, std::size_t> idToRecordId;

public:
    // contructs utility object to manage binary file filled with given array of employees
    Controller(const std::string &binaryFileName, const Employee *employees, const std::size_t &size, bool &ok);

    // gets record by given id and puts it into employee variable, returns false in case there is no employee under such id
    bool getRecord(const std::size_t &id, Employee &employee) const;

    // sets record by oldId to employee, returns false in case there is no employee under oldId or newId(employee.id) is taken
    bool setRecord(const std::size_t &oldId, const Employee &employee) const;
};