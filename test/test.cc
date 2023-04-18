#include <gtest/gtest.h>
#include <string>
#include "model.h"

TEST(TestModelReadRecord, TestOnlyOneRecord)
{
    constexpr size_t size = 1;
    Employee firstEmployee = {1, "vasya", 5.2};
    Employee employees[size] = {firstEmployee};
    const std::string binaryFileName = "testbinary";
    Model model(binaryFileName);
    model.writeBinaryFile(employees, size);
    EXPECT_EQ(firstEmployee, model.readRecord(0));
}