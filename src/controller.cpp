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
    Utility::fillMap(_idToRecordId, employees, size);
    model.writeBinaryFile(employees, size);
}

bool Controller::getRecord(const int &id, Employee &employee) const
{
    try
    {
        std::size_t recordId = _idToRecordId.at(id);
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
        std::size_t recordId = _idToRecordId.at(oldId);
        int newId = employee.id;
        _idToRecordId.erase(oldId);
        try
        {
            _idToRecordId.at(newId);
            // thereIsRecord under newId
            return false;
        }
        catch (const std::exception &e)
        {
            _idToRecordId[newId] = recordId;
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

bool Controller::idToRecordId(const int& id, size_t& recordId) const
{
    try
    {
        std::size_t _recordId = _idToRecordId.at(id);
        recordId = _recordId;
        return true;
    }
    catch (const std::exception &e)
    {
        // there is no record under oldId
        return false;
    }
}

Employee* Controller::getAllRecords(size_t& size) const
{
    size = _idToRecordId.size();
    Employee *array = new Employee[size];

    size_t i = 0;
    for (const auto& key_value : _idToRecordId)
    {
        Controller::getRecord(key_value.first, array[i]);
        ++i;
    }
    
    return array;
}