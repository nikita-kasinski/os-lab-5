//
// Created by Nikita Kasinski
//

#include "utility.h"
#include <set>

bool Utility::hasEquals(const int *array, const size_t &size)
{
    std::set<int> distinctElements;
    for (size_t i = 0; i < size; ++i)
    {
        distinctElements.insert(array[i]);
    }
    return (distinctElements.size() == size);
}

int *Utility::extractIds(const Employee *employees, const size_t &size)
{
    int *ids = new int[size];
    for (size_t i = 0; i < size; ++i)
    {
        ids[i] = employees[i].id;
    }
    return ids;
}