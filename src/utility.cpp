//
// Created by Nikita Kasinski
//

#include "utility.h"
#include <set>

Utility::Utility(const std::string &binaryFileName, const Employee *employees, const size_t &size, bool &ok)
{
    int *ids = extractIds(employees, size);
    bool hasEq = hasEquals(ids, size);
    delete[] ids;
    if (hasEq)
    {
        ok = false; // as array has equal ids correct we exit process
    }
    else
    {
        ok = true;
    }
    std::fstream fout(binaryFileName.c_str(), std::ios::binary | std::ios::out);
    fout.seekp(0);
    fout.write(reinterpret_cast<const char *>(&size), sizeof(size_t));
    for (size_t i = 0; i < size; ++i)
    {
        fout.write(reinterpret_cast<const char *>(employees), sizeof(employees));
    }
}

bool Utility::getRecord(std::istream &f, const size_t &id, Employee &employee) const
{
    // size_t numberOfRecords = Utility::getNumberOfRecords();
    // size_t maxId = numberOfRecords - 1; // -1 as id are in range
}

bool Utility::hasEquals(const int *array, const size_t &size) const
{
    std::set<int> distinctElements;
    for (size_t i = 0; i < size; ++i)
    {
        distinctElements.insert(array[i]);
    }
    return (distinctElements.size() == size);
}

int *Utility::extractIds(const Employee *employees, const size_t &size) const
{
    int *ids = new int[size];
    for (size_t i = 0; i < size; ++i)
    {
        ids[i] = employees[i].id;
    }
    return ids;
}

void Utility::fillMap(const Employee* employees, const size_t& size)
{
    idToRecordId.clear();
    for (size_t i = 0; i < size; ++i)
    {
        idToRecordId[employees[i].id] = i;
    }
}