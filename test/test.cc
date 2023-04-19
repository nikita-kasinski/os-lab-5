#include <gtest/gtest.h>
#include <string>

#define private public

#include "model.h"
#include "utility.h"
#include "controller.h"


TEST(Model_readRecord, OnlyOneRecord)
{
    constexpr std::size_t size = 1;
    Employee firstEmployee = {1, "vasya", 5.2};
    Employee employees[size] = {firstEmployee};
    const std::string binaryFileName = "testbinary";
    Model model(binaryFileName);
    model.writeBinaryFile(employees, size);
    EXPECT_EQ(firstEmployee, model.readRecord(0));
}

TEST(Model_readRecord, SeveralRecords)
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
TEST(Model_writeRecord, OneRecord)
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

TEST(Model_writeRecord, SeveralRecords)
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

TEST(Utility_hasEquals, NoEquals)
{
    constexpr std::size_t size = 4;
    int array[size] = {1, 2, 3, 4};
    EXPECT_FALSE(Utility::hasEquals(array, size));
}

TEST(Utility_hasEquals, ZeroElems)
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

TEST(Utility_hasEquals, TwoEquals)
{
    constexpr std::size_t size = 5;
    int array[size] = {1, 2, 3, 4, 1};
    EXPECT_TRUE(Utility::hasEquals(array, size));
}

TEST(Utility_hasEquals, AllEquals)
{
    constexpr std::size_t size = 5;
    int array[size] = {1, 1, 1, 1, 1};
    EXPECT_TRUE(Utility::hasEquals(array, size));
}

TEST(Utility_extractIds, OneElem)
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
TEST(Utility_extractIds, severalElems)
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

TEST(Utility_fillMap, StandartFlow)
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

TEST(Controller_getRecord, TestExistingId)
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
    Employee employeeRead;
    EXPECT_TRUE(ctrl.getRecord(1, employeeRead));
    EXPECT_EQ(employeeRead, firstEmployee);
    EXPECT_TRUE(ctrl.getRecord(3, employeeRead));
    EXPECT_EQ(employeeRead, thirdEmployee);
}

TEST(Controller_getRecord, TestFictitiousId)
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
    Employee employeeRead;
    EXPECT_FALSE(ctrl.getRecord(7, employeeRead));
}

TEST(Controller_writeRecord, FictitiousId)
{
    constexpr std::size_t size = 5;
    Employee firstEmployee = {1, "vasya", 4.8};
    Employee secondEmployee = {2, "petya", 5.2};
    Employee thirdEmployee = {3, "kostya", 7};
    Employee forthEmployee = {4, "vitya", 10};
    Employee fifthEmployee = {5, "sasha", 12};
    Employee newEmployee = {3, "ilya", 20};
    Employee employees[size] = {firstEmployee, secondEmployee, thirdEmployee, forthEmployee, fifthEmployee};
    const std::string binaryFileName = "testbinary";
    bool ok;
    Controller ctrl(binaryFileName, employees, size, ok);
    EXPECT_TRUE(ok);

    EXPECT_FALSE(ctrl.setRecord(7, newEmployee));
}

TEST(Controller_writeRecord, SameIdOverwrite)
{
    constexpr std::size_t size = 5;
    Employee firstEmployee = {1, "vasya", 4.8};
    Employee secondEmployee = {2, "petya", 5.2};
    Employee thirdEmployee = {3, "kostya", 7};
    Employee forthEmployee = {4, "vitya", 10};
    Employee fifthEmployee = {5, "sasha", 12};
    Employee newEmployee = {3, "ilya", 20};
    Employee employees[size] = {firstEmployee, secondEmployee, thirdEmployee, forthEmployee, fifthEmployee};
    const std::string binaryFileName = "testbinary";
    bool ok;
    Controller ctrl(binaryFileName, employees, size, ok);
    EXPECT_TRUE(ok);
    Employee employeeRead;

    EXPECT_TRUE(ctrl.getRecord(3, employeeRead));
    EXPECT_EQ(thirdEmployee, employeeRead);

    EXPECT_TRUE(ctrl.setRecord(3, newEmployee));
    EXPECT_TRUE(ctrl.getRecord(3, employeeRead));
    EXPECT_EQ(newEmployee, employeeRead);
}

TEST(Controller_writeRecord, NewIdWrite)
{
    constexpr std::size_t size = 5;
    Employee firstEmployee = {1, "vasya", 4.8};
    Employee secondEmployee = {2, "petya", 5.2};
    Employee thirdEmployee = {3, "kostya", 7};
    Employee forthEmployee = {4, "vitya", 10};
    Employee fifthEmployee = {5, "sasha", 12};
    Employee newEmployee = {7, "ilya", 20};
    Employee employees[size] = {firstEmployee, secondEmployee, thirdEmployee, forthEmployee, fifthEmployee};
    const std::string binaryFileName = "testbinary";
    bool ok;
    Controller ctrl(binaryFileName, employees, size, ok);
    EXPECT_TRUE(ok);
    Employee employeeRead;

    EXPECT_TRUE(ctrl.setRecord(4, newEmployee));
    EXPECT_FALSE(ctrl.getRecord(4, employeeRead));

    EXPECT_TRUE(ctrl.getRecord(7, employeeRead));
    EXPECT_EQ(newEmployee, employeeRead);
}

TEST(Controller_writeRecord, NewIdIsTaken)
{
    constexpr std::size_t size = 5;
    Employee firstEmployee = {1, "vasya", 4.8};
    Employee secondEmployee = {2, "petya", 5.2};
    Employee thirdEmployee = {3, "kostya", 7};
    Employee forthEmployee = {4, "vitya", 10};
    Employee fifthEmployee = {5, "sasha", 12};
    Employee newEmployee = {2, "ilya", 20};
    Employee employees[size] = {firstEmployee, secondEmployee, thirdEmployee, forthEmployee, fifthEmployee};
    const std::string binaryFileName = "testbinary";
    bool ok;
    Controller ctrl(binaryFileName, employees, size, ok);
    EXPECT_TRUE(ok);

    EXPECT_FALSE(ctrl.setRecord(4, newEmployee));
}

TEST(Utility_safeUnsignedIntegerInput, StandardInput)
{
    std::istringstream testStream("1 2 3 4\n");
    std::ostringstream ostream;
    size_t a, b, c, d;
    a = Utility::safeUnsignedIntegerInput(testStream, ostream, "", "");
    b = Utility::safeUnsignedIntegerInput(testStream, ostream, "", "");
    c = Utility::safeUnsignedIntegerInput(testStream, ostream, "", "");
    d = Utility::safeUnsignedIntegerInput(testStream, ostream, "", "");
    EXPECT_EQ(a, 1);
    EXPECT_EQ(b, 2);
    EXPECT_EQ(c, 3);
    EXPECT_EQ(d, 4);
}

TEST(Utility_safeUnsignedIntegerInput, FailedInput)
{
    std::istringstream testStream("asdfbsda1\n2 sadgsadg123gasd\n 3\n");
    std::ostringstream ostream;
    size_t a, b;
    a = Utility::safeUnsignedIntegerInput(testStream, ostream, "", "");
    b = Utility::safeUnsignedIntegerInput(testStream, ostream, "", "");
    EXPECT_EQ(a, 2);
    EXPECT_EQ(b, 3);
}

TEST(Utility_safeUnsignedIntegerInput, NegativeInput)
{
    std::istringstream testStream("skldghuhasdkjgn\n -1 \n2\n");
    std::ostringstream ostream;
    size_t a;
    a = Utility::safeUnsignedIntegerInput(testStream, ostream, "", "");
    EXPECT_EQ(a, 2);
}

TEST(Utility_readEmployee, StandartFlow)
{
    std::istringstream testStream("1 vasya 1\n2 petya 2");
    std::ostringstream out;
    Employee firstEmployee = {1, "vasya", 1};
    Employee secondEmployee = {2, "petya", 2};
    EXPECT_EQ(firstEmployee, Utility::readEmployee(testStream, out));
    EXPECT_EQ(secondEmployee, Utility::readEmployee(testStream, out));
}

TEST(Utility_printEmployee, StandartFlow)
{
    std::ostringstream out;
    std::ostringstream expectedOut;
    Employee firstEmployee = {1, "vasya", 1};
    Employee secondEmployee = {2, "petya", 2};
    Employee employees[2] = {firstEmployee, secondEmployee};
    for (size_t i = 0; i < 2; ++i)
    {
        expectedOut
            << std::setw(Utility::idWidth) << employees[i].id
            << std::setw(Utility::nameWidth) << employees[i].name
            << std::setw(Utility::hoursWidth) << employees[i].hours
            << "\n";
    }
    
    Utility::printEmployee(out, firstEmployee);
    Utility::printEmployee(out, secondEmployee);

    EXPECT_EQ(expectedOut.str(), out.str());
}

TEST(Utility_printEmployees, StandartFlow)
{
    std::ostringstream out;
    std::ostringstream expectedOut;
    Employee firstEmployee = {1, "vasya", 1};
    Employee secondEmployee = {2, "petya", 2};
    Employee employees[2] = {firstEmployee, secondEmployee};
    expectedOut
            << std::setw(Utility::idWidth) << "id"
            << std::setw(Utility::nameWidth) << "name"
            << std::setw(Utility::hoursWidth) << "hours"
            << "\n";
    for (size_t i = 0; i < 2; ++i)
    {
        expectedOut
            << std::setw(Utility::idWidth) << employees[i].id
            << std::setw(Utility::nameWidth) << employees[i].name
            << std::setw(Utility::hoursWidth) << employees[i].hours
            << "\n";
    }
    
    Utility::printEmployees(out, employees, 2);

    EXPECT_EQ(expectedOut.str(), out.str());
}

TEST(Utility_getWriteEventName, StandartFlow)
{
    std::string expectedEventName = "Write event 1";
    EXPECT_EQ(expectedEventName, Utility::getWriteEventName(1));
}

TEST(Utility_getReadEventName, StandartFlow)
{
    std::string expectedEventName = "Read event 1 2";
    EXPECT_EQ(expectedEventName, Utility::getReadEventName(1, 2));
}

TEST(Controller_idToRecordId, FictitiousId)
{
    constexpr std::size_t size = 5;
    Employee firstEmployee = {7, "vasya", 4.8};
    Employee secondEmployee = {15, "petya", 5.2};
    Employee thirdEmployee = {2, "kostya", 7};
    Employee forthEmployee = {9, "vitya", 10};
    Employee fifthEmployee = {28, "sasha", 12};
    Employee employees[size] = {firstEmployee, secondEmployee, thirdEmployee, forthEmployee, fifthEmployee};
    const std::string binaryFileName = "testbinary";
    bool ok;
    Controller ctrl(binaryFileName, employees, size, ok);
    EXPECT_TRUE(ok);

    size_t recordId;
    EXPECT_FALSE(ctrl.idToRecordId(5, recordId));
}

TEST(Controller_idToRecordId, StandartFlow)
{
    constexpr std::size_t size = 5;
    Employee firstEmployee = {7, "vasya", 4.8};
    Employee secondEmployee = {15, "petya", 5.2};
    Employee thirdEmployee = {2, "kostya", 7};
    Employee forthEmployee = {9, "vitya", 10};
    Employee fifthEmployee = {28, "sasha", 12};
    Employee employees[size] = {firstEmployee, secondEmployee, thirdEmployee, forthEmployee, fifthEmployee};
    const std::string binaryFileName = "testbinary";
    bool ok;
    Controller ctrl(binaryFileName, employees, size, ok);
    EXPECT_TRUE(ok);

    size_t recordId;
    EXPECT_TRUE(ctrl.idToRecordId(7, recordId));
    EXPECT_EQ(recordId, 0);

    EXPECT_TRUE(ctrl.idToRecordId(15, recordId));
    EXPECT_EQ(recordId, 1);

    EXPECT_TRUE(ctrl.idToRecordId(2, recordId));
    EXPECT_EQ(recordId, 2);

    EXPECT_TRUE(ctrl.idToRecordId(9, recordId));
    EXPECT_EQ(recordId, 3);

    EXPECT_TRUE(ctrl.idToRecordId(28, recordId));
    EXPECT_EQ(recordId, 4);
}

TEST(Controler_getAllRecords, StandartFlow)
{
    constexpr std::size_t size = 5;
    Employee firstEmployee = {7, "vasya", 4.8};
    Employee secondEmployee = {15, "petya", 5.2};
    Employee thirdEmployee = {2, "kostya", 7};
    Employee forthEmployee = {9, "vitya", 10};
    Employee fifthEmployee = {28, "sasha", 12};
    Employee employees[size] = {thirdEmployee, firstEmployee, forthEmployee, secondEmployee, fifthEmployee};
    const std::string binaryFileName = "testbinary";
    bool ok;
    Controller ctrl(binaryFileName, employees, size, ok);
    EXPECT_TRUE(ok);

    size_t returnedSize;
    Employee *returnedEmployees = ctrl.getAllRecords(returnedSize);

    ASSERT_EQ(returnedSize, size);
    for (size_t i = 0; i < size; ++i)
    {
        EXPECT_EQ(employees[i], returnedEmployees[i]);
    }
    delete[] returnedEmployees;
}