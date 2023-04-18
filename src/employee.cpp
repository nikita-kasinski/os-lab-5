//
// Created by Nikita Kasinski
//

#include <cstring>
#include "employee.h"

bool operator==(const Employee& lhs, const Employee& rhs)
{
    bool eqIds = (lhs.id == rhs.id);
    bool eqNames = (strcmp(lhs.name, rhs.name) == 0);
    bool eqHours = (lhs.hours == rhs.hours);
    return (eqIds && eqNames && eqHours);
}