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

public:
    // contructs utility object to manage binary file filled with given array of employees
    Controller(const std::string &binaryFileName, const Employee *employees, const size_t &size, bool &ok);

    // gets record by given id and puts it into employee, return false in case there is no employee under such id
    bool getRecord(std::istream &f, const size_t &id, Employee &employee) const;
};