//
// Created by Nikita Kasinski
//

#include "controller.h"
#include "utility.h"

Controller::Controller(
    const std::string &binaryFileName,
    const std::vector<Employee>& employees,
    bool &ok)
    : model(binaryFileName)
{
    std::vector<int> ids = Utility::extractIds(employees);
    bool hasEquals = Utility::hasEquals(ids);
    if (hasEquals)
    {
        ok = false; // as array has equal ids we exit process
        return;
    }
    ok = true;
    Utility::fillMap(_idToRecordId, employees);
    model.writeBinaryFile(employees);
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

std::vector<Employee> Controller::getAllRecords() const
{
    std::vector<Employee> result(_idToRecordId.size());

    size_t i = 0;
    for (const auto& key_value : _idToRecordId)
    {
        Controller::getRecord(key_value.first, result[i]);
        ++i;
    }
    
    return result;
}