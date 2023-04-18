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
        return;
    }
    ok = true;
    Utility::fillMap(idToRecordId, employees, size);
    model.writeBinaryFile(employees, size);
}

bool Controller::getRecord(const int &id, Employee &employee) const
{
    try
    {
        std::size_t recordId = idToRecordId.at(id);
        employee = model.readRecord(recordId);
        return true;
    }
    catch (const std::exception &e)
    {
        return false;
    }
}

bool Controller::setRecord(const int& oldId, const Employee &employee)
{
    try
    {
        std::size_t recordId = idToRecordId.at(oldId);
        int newId = employee.id;
        idToRecordId.erase(oldId);
        try
        {
            idToRecordId.at(newId);
            // thereIsRecord under newId
            return false;
        }
        catch (const std::exception &e)
        {
            idToRecordId[newId] = recordId;
            model.writeRecord(recordId, employee);
            return true;
        }
    }
    catch (const std::exception &e)
    {
        // there is no record under oldId
        return false;
    }
}