//
// Created by Nikita Kasinski
//

#include <iostream>
#include "utility.h"
#include "controller.h"

int main()
{
    // retrieving number of employees
    size_t employeesSize = Utility::safeUnsignedIntegerInput(
        std::cin,
        std::cout,
        "Enter number of employees: ",
        "Value must be positive integer\n");

    // retrieving employees
    Employee *employees = new Employee[employeesSize];
    for (size_t i = 0; i < employeesSize; ++i)
    {
        employees[i] = Utility::readEmployee(std::cin, std::cout);
    }

    // retrieving binary file name
    std::string binaryFileName;
    std::cin >> binaryFileName;

    // creating controller for binary file
    bool ok = false;
    Controller ctrl(binaryFileName, employees, employeesSize, ok);
    if (!ok)
    {
        std::cerr << "Employees array has equal ids. Quit\n";
        return 1;
    }

    // printing binary file
    {
        size_t employeesFromFileSize;
        Employee *employeesFromFile = ctrl.getAllRecords(employeesFromFileSize);
        Utility::printEmployees(std::cout, employeesFromFile, employeesFromFileSize);
        delete[] employeesFromFile;
    }
    
    // retrieving number of clients
    size_t numberOfClients = Utility::safeUnsignedIntegerInput(
        std::cin,
        std::cout,
        "Enter number of clients: ",
        "Value must be positive integer\n");

    // freeing memory
    delete[] employees;
    return 0;
}