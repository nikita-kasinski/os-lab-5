//
// Created by Nikita Kasinski
//

#include <set>
#include <iomanip>
#include "utility.h"

bool Utility::hasEquals(const int *array, const std::size_t &size)
{
    std::set<int> distinctElements;
    for (std::size_t i = 0; i < size; ++i)
    {
        distinctElements.insert(array[i]);
    }
    return (distinctElements.size() != size);
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

Employee Utility::readEmployee(std::istream &in, std::ostream &out)
{
    std::string employeePrompt = "Enter employee data in format: id name hours\n";
    Employee employeeRead;
    out << employeePrompt;
    in >> employeeRead.id >> employeeRead.name >> employeeRead.hours;
    return employeeRead;
}

void Utility::printEmployee(std::ostream &out, const Employee &employee)
{
    out
        << std::setw(Utility::idWidth) << employee.id
        << std::setw(Utility::nameWidth) << employee.name
        << std::setw(Utility::hoursWidth) << employee.hours
        << "\n";
}

size_t Utility::safeUnsignedIntegerInput(std::istream &in, std::ostream &out, const std::string &inputPrompt, const std::string &inputFailedPrompt)
{
    constexpr auto maxStreamSize = std::numeric_limits<std::streamsize>::max();
    int tempAns = -1;
    bool failed = false;

    while (tempAns <= 0)
    {
        if (failed)
        {
            out << inputFailedPrompt;
        }
        else
        {
            failed = true;
        }
        out << inputPrompt;
        in >> tempAns;
        if (!in.good())
        {
            in.clear();
            in.ignore(maxStreamSize, '\n');
            tempAns = -1;
        }
    }
    return static_cast<size_t>(tempAns);
}

void Utility::printEmployees(std::ostream &out, const Employee *array, const size_t &size)
{
    // print header
    out
        << std::setw(Utility::idWidth) << "id"
        << std::setw(Utility::nameWidth) << "name"
        << std::setw(Utility::hoursWidth) << "hours"
        << "\n";

    // print employees
    for (size_t i = 0; i < size; ++i)
    {
        printEmployee(out, array[i]);
    }
}

std::string Utility::getReadEventName(const std::size_t &recordId, const std::size_t &clientId)
{
    std::string eventNameTemplate = "Read event";
    std::ostringstream eventName;
    eventName << eventNameTemplate << ' ' << recordId << ' ' << clientId;
    return eventName.str();
}

std::string Utility::getWriteEventName(const std::size_t &recordId)
{
    std::string eventNameTemplate = "Write event";
    std::ostringstream eventName;
    eventName << eventNameTemplate << ' ' << recordId;
    return eventName.str();
}