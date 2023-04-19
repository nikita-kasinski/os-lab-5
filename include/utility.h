#pragma once
//
// Created by Nikita Kasinski
//

#include <limits>
#include <fstream>
#include <string>
#include <map>
#include "employee.h"

class Utility
{
private:
    static constexpr auto maxStreamSize = std::numeric_limits<std::streamsize>::max();
    static constexpr size_t idWidth = 5;
    static constexpr size_t nameWidth = 15;
    static constexpr size_t hoursWidth = 5;

public:
    // checks if the array has equal elements
    static bool hasEquals(const int *array, const std::size_t &size);

    // creates array of ids of given employees, returned array must be freed
    static int *extractIds(const Employee *employees, const std::size_t &size);

    // maps Employee.id -> position in the binary file in mapToFill 
    static void fillMap(std::map<int, std::size_t> &mapToFill, const Employee *employess, const std::size_t &size);

    // reads employee from in, puts input prompts into out
    static Employee readEmployee(std::istream& in, std::ostream& out);

    // prints employee to out
    static void printEmployee(std::ostream& out, const Employee& employee);

    // safely inputs size_t value from in, puts input prompts into out
    static size_t safeUnsignedIntegerInput(
        std::istream &in, 
        std::ostream &out, 
        const std::string &inputPrompt, 
        const std::string &inputFailedPrompt);

    // outputs employee array into out
    static void printEmployees(std::ostream& out, const Employee* employees, const std::size_t& size);

    static std::string getWriteEventName(const std::size_t& recordId);

    static std::string getReadEventName(const std::size_t& recordId, const std::size_t& clientId);

};