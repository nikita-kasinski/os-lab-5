//
// Created by Nikita Kasinski
//

#include "utility.h"

void Utility::initBinaryFile(const std::string& binaryFileName, const Employee* employees, const size_t& size)
{
    std::fstream fout(binaryFileName.c_str(), std::ios::binary | std::ios::out);
    fout.seekp(0);
    fout.write(reinterpret_cast<const char*>(&size), sizeof(size_t));
    for (size_t i = 0; i < size; ++i)
    {
        fout.write(reinterpret_cast<const char*>(employees), sizeof(employees));
    }
}