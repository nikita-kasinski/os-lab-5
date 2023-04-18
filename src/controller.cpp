//
// Created by Nikita Kasinski
//

#include "controller.h"
#include "utility.h"

Controller::Controller(
    const std::string &binaryFileName,
    const Employee *employees,
    const size_t &size,
    bool &ok)
    : model(binaryFileName)
{
    int *ids = Utility::extractIds(employees, size);
    bool hasEq = Utility::hasEquals(ids, size);
    delete[] ids;
    if (hasEq)
    {
        ok = false; // as array has equal ids correct we exit process
    }
    else
    {
        ok = true;
        model.fillMap(employees, size);
        model.writeBinaryFile(employees, size);
    }
}