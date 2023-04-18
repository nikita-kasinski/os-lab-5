//
// Created by Nikita Kasinski
//

#include "controller.h"
#include "utility.h"

Controller::Controller(
    const std::string &binaryFileName,
    const Employee *employees,
    const std::size_t &size,
    bool &ok)
    : model(binaryFileName)
{
    int *ids = Utility::extractIds(employees, size);
    bool hasEq = Utility::hasEquals(ids, size);
    delete[] ids;
    if (hasEq)
    {
        ok = false; // as array has equal ids we exit process
    }
    else
    {
        ok = true;
        Utility::fillMap(idToRecordId, employees, size);
        model.writeBinaryFile(employees, size);
    }
}

bool Controller::getRecord(const std::size_t& id, Employee& employee) const
{
    
}