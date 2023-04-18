#include <gtest/gtest.h>
#include <string>
#include "model.h"

TEST(TestModelReadRecord, TestOnlyOneRecord)
{
    constexpr std::size_t size = 1;
    Employee firstEmployee = {1, "vasya", 5.2};
    Employee employees[size] = {firstEmployee};
    const std::string binaryFileName = "testbinary";
    Model model(binaryFileName);
    model.writeBinaryFile(employees, size);
    EXPECT_EQ(firstEmployee, model.readRecord(0));
}

TEST(TestModelReadRecord, TestSeveralRecords)
{
    constexpr std::size_t size = 3;
    Employee firstEmployee = {1, "vasya", 4.8};
    Employee secondEmployee = {2, "petya", 5.2};
    Employee thirdEmployee = {3, "kostya", 7};
    Employee employees[size] = {firstEmployee, secondEmployee, thirdEmployee};
    const std::string binaryFileName = "testbinary";
    Model model(binaryFileName);
    model.writeBinaryFile(employees, size);
    EXPECT_EQ(firstEmployee, model.readRecord(0));
    EXPECT_EQ(secondEmployee, model.readRecord(1));
    EXPECT_EQ(thirdEmployee, model.readRecord(2));
    EXPECT_EQ(secondEmployee, model.readRecord(1));
    EXPECT_EQ(firstEmployee, model.readRecord(0));
    EXPECT_EQ(thirdEmployee, model.readRecord(2));
}

// it is vital to write tests with same id and different ids for controller, model doesn't care but controller does
TEST(TestModelWriteRecord, TestOneRecord)
{
    constexpr std::size_t size = 1;
    Employee firstEmployee = {1, "vasya", 4.8};
    Employee newFirstEmployee = {1, "petya", 5.2};
    Employee employees[size] = {firstEmployee};
    const std::string binaryFileName = "testbinary";
    Model model(binaryFileName);
    model.writeBinaryFile(employees, size);
    EXPECT_EQ(firstEmployee, model.readRecord(0));
    model.writeRecord(0, newFirstEmployee);
    EXPECT_EQ(newFirstEmployee, model.readRecord(0));
}

TEST(TestModelWriteRecord, TestSeveralRecords)
{
    constexpr std::size_t size = 3;
    Employee firstEmployee = {1, "vasya", 4.8};
    Employee secondEmployee = {2, "petya", 5.2};
    Employee thirdEmployee = {3, "kostya", 7};
    Employee newSecondEmployee = {4, "vitya", 10};
    Employee newThirdEmployee = {5, "sasha", 12};
    Employee employees[size] = {firstEmployee, secondEmployee, thirdEmployee};
    const std::string binaryFileName = "testbinary";
    Model model(binaryFileName);
    model.writeBinaryFile(employees, size);
    EXPECT_EQ(firstEmployee, model.readRecord(0));
    EXPECT_EQ(secondEmployee, model.readRecord(1));
    EXPECT_EQ(thirdEmployee, model.readRecord(2));

    model.writeRecord(1, newSecondEmployee);
    EXPECT_EQ(newSecondEmployee, model.readRecord(1));

    model.writeRecord(1, secondEmployee);
    EXPECT_EQ(secondEmployee, model.readRecord(1));

    model.writeRecord(2, newThirdEmployee);
    EXPECT_EQ(newThirdEmployee, model.readRecord(2));
}