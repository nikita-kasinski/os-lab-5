//
// Created by Nikita Kasinski
//

#include "model.h"

Model::Model(const std::string &binaryFileName)
{
    this->binaryFileName = binaryFileName;
}

void Model::writeBinaryFile(const Employee *employees, const std::size_t &size) const
{
    std::fstream fout(binaryFileName.c_str(), std::ios::binary | std::ios::out);
    fout.seekp(0);
    for (std::size_t i = 0; i < size; ++i)
    {
        fout.write(reinterpret_cast<const char *>(employees), sizeof(employees));
    }
}

bool Model::getRecord(const std::size_t &id, Employee &employee) const
{
}