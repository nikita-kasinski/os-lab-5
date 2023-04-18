//
// Created by Nikita Kasinski
//

#include <iostream>
#include "utility.h"
#include "controller.h"

int main()
{
    // retrieving number of employees
    size_t size = Utility::safeUnsignedIntegerInput(
        std::cin,
        std::cout, 
        "Enter number of employees: ",
        "Value must be positive integer\n");

    // retrieving employees
    Employee *employees = new Employee[size];
    for (size_t i = 0; i < size; ++i)
    {
        employees[i] = Utility::readEmployee(std::cin, std::cout);
    }

    // retrieving binary file name
    std::string binaryFileName;
    std::cin >> binaryFileName;

    
    // freeing memory
    delete[] employees;
    return 0;
}