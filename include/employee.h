//
// Created by Nikita Kasinski
//

#pragma once

struct Employee
{
    int id;
    char name[10];
    double hours; // hours worked
};

bool operator==(const Employee& lhs, const Employee& rhs);