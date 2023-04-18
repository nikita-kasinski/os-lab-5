//
// Created by Nikita Kasinski
//

#include "model.h"

Model::Model(const std::string &binaryFileName)
{
    this->binaryFileName = binaryFileName;
}
void Model::fillMap(const Employee *employees, const size_t &size)
{
    idToRecordId.clear();
    for (size_t i = 0; i < size; ++i)
    {
        idToRecordId[employees[i].id] = i;
    }
}

void Model::writeBinaryFile(const Employee *employees, const size_t &size) const
{
    std::fstream fout(binaryFileName.c_str(), std::ios::binary | std::ios::out);
    fout.seekp(0);
    for (size_t i = 0; i < size; ++i)
    {
        fout.write(reinterpret_cast<const char *>(employees), sizeof(employees));
    }
}