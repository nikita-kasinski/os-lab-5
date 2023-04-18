#include "utility.h"

//
// Created by Nikita Kasinski
//

#include "utility.h"
#include <set>

bool Utility::hasEquals(const int *array, const std::size_t &size)
{
    std::set<int> distinctElements;
    for (std::size_t i = 0; i < size; ++i)
    {
        distinctElements.insert(array[i]);
    }
    return (distinctElements.size() == size);
}

int *Utility::extractIds(const Employee *employees, const std::size_t &size)
{
    int *ids = new int[size];
    for (std::size_t i = 0; i < size; ++i)
    {
        ids[i] = employees[i].id;
    }
    return ids;
}

void Utility::fillMap(std::map<int, std::size_t> &mapToFill, const Employee *employees, const std::size_t &size)
{
    mapToFill.clear();
    for (std::size_t i = 0; i < size; ++i)
    {
        mapToFill[employees[i].id] = i;
    }
}