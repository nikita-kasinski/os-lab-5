#include <gtest/gtest.h>
#include <string>
#include "model.h"
#include "utility.h"
#include "controller.h"

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

TEST(TestUtilityHasEquals, TestNoEquals)
{
    constexpr std::size_t size = 4;
    int array[size] = {1, 2, 3, 4};
    EXPECT_FALSE(Utility::hasEquals(array, size));
}

TEST(TestUtilityHasEquals, ZeroElems)
{
    constexpr std::size_t size = 0;
    int *array = nullptr;
    EXPECT_FALSE(Utility::hasEquals(array, size));
}

TEST(TestUtilityHasEquals, OneElem)
{
    constexpr std::size_t size = 1;
    int array[size] = {1};
    EXPECT_FALSE(Utility::hasEquals(array, size));
}

TEST(TestUtilityHasEquals, TwoEquals)
{
    constexpr std::size_t size = 5;
    int array[size] = {1, 2, 3, 4, 1};
    EXPECT_TRUE(Utility::hasEquals(array, size));
}

TEST(TestUtilityHasEquals, AllEquals)
{
    constexpr std::size_t size = 5;
    int array[size] = {1, 1, 1, 1, 1};
    EXPECT_TRUE(Utility::hasEquals(array, size));
}

TEST(TestUtilityExtractIds, oneElem)
{
    constexpr std::size_t size = 1;
    Employee firstEmployee = {1, "vasya", 4.8};
    Employee employees[size] = {firstEmployee};
    int *ids = Utility::extractIds(employees, size);
    for (size_t i = 0; i < size; ++i)
    {
        EXPECT_EQ(ids[i], employees[i].id);
    }
}
TEST(TestUtilityExtractIds, SeveralElems)
{
    constexpr std::size_t size = 5;
    Employee firstEmployee = {1, "vasya", 4.8};
    Employee secondEmployee = {2, "petya", 5.2};
    Employee thirdEmployee = {3, "kostya", 7};
    Employee forthEmployee = {4, "vitya", 10};
    Employee fifthEmployee = {5, "sasha", 12};
    Employee employees[size] = {firstEmployee, secondEmployee, thirdEmployee, forthEmployee, fifthEmployee};
    int *ids = Utility::extractIds(employees, size);
    for (size_t i = 0; i < size; ++i)
    {
        EXPECT_EQ(ids[i], employees[i].id);
    }
    delete[] ids;
}

TEST(TestUtilityFillMap, TestStandartFlow)
{
    constexpr std::size_t size = 5;
    Employee firstEmployee = {5, "vasya", 4.8};
    Employee secondEmployee = {1, "petya", 5.2};
    Employee thirdEmployee = {3, "kostya", 7};
    Employee forthEmployee = {2, "vitya", 10};
    Employee fifthEmployee = {4, "sasha", 12};
    Employee employees[size] = {firstEmployee, secondEmployee, thirdEmployee, forthEmployee, fifthEmployee};
    std::map<int, size_t> expectedAnswer;
    expectedAnswer[5] = 0;
    expectedAnswer[1] = 1;
    expectedAnswer[3] = 2;
    expectedAnswer[2] = 3;
    expectedAnswer[4] = 4;
    std::map<int, size_t> filledMap;
    Utility::fillMap(filledMap, employees, size);
    EXPECT_EQ(expectedAnswer, filledMap);
}

TEST(Controller_Controller, TestDistinctIds)
{
    constexpr std::size_t size = 5;
    Employee firstEmployee = {1, "vasya", 4.8};
    Employee secondEmployee = {2, "petya", 5.2};
    Employee thirdEmployee = {3, "kostya", 7};
    Employee forthEmployee = {4, "vitya", 10};
    Employee fifthEmployee = {5, "sasha", 12};
    Employee employees[size] = {firstEmployee, secondEmployee, thirdEmployee, forthEmployee, fifthEmployee};
    const std::string binaryFileName = "testbinary";
    bool ok;
    Controller ctrl(binaryFileName, employees, size, ok);
    EXPECT_TRUE(ok);
}

TEST(Controller_Controller, TestEqualIds)
{
    constexpr std::size_t size = 5;
    Employee firstEmployee = {1, "vasya", 4.8};
    Employee secondEmployee = {2, "petya", 5.2};
    Employee thirdEmployee = {1, "kostya", 7};
    Employee forthEmployee = {4, "vitya", 10};
    Employee fifthEmployee = {5, "sasha", 12};
    Employee employees[size] = {firstEmployee, secondEmployee, thirdEmployee, forthEmployee, fifthEmployee};
    const std::string binaryFileName = "testbinary";
    bool ok;
    Controller ctrl(binaryFileName, employees, size, ok);
    EXPECT_FALSE(ok);
}