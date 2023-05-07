#include <gtest/gtest.h>
#include <string>

#define private public

#include "model.h"
#include "utility.h"
#include "controller.h"
#include "args.h"

TEST(Model_readRecord, OnlyOneRecord)
{
    Employee firstEmployee = {1, "vasya", 5.2};
    std::vector<Employee> employees = {firstEmployee};
    const std::string binaryFileName = "testbinary";
    Model model(binaryFileName);
    model.writeBinaryFile(employees);
    EXPECT_EQ(firstEmployee, model.readRecord(0));
}

TEST(Model_readRecord, SeveralRecords)
{
    Employee firstEmployee = {1, "vasya", 4.8};
    Employee secondEmployee = {2, "petya", 5.2};
    Employee thirdEmployee = {3, "kostya", 7};
    std::vector<Employee> employees = {firstEmployee, secondEmployee, thirdEmployee};
    const std::string binaryFileName = "testbinary";
    Model model(binaryFileName);
    model.writeBinaryFile(employees);
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
    Employee firstEmployee = {1, "vasya", 4.8};
    Employee newFirstEmployee = {1, "petya", 5.2};
    std::vector<Employee> employees = {firstEmployee};
    const std::string binaryFileName = "testbinary";
    Model model(binaryFileName);
    model.writeBinaryFile(employees);
    EXPECT_EQ(firstEmployee, model.readRecord(0));
    model.writeRecord(0, newFirstEmployee);
    EXPECT_EQ(newFirstEmployee, model.readRecord(0));
}

TEST(Model_writeRecord, SeveralRecords)
{
    Employee firstEmployee = {1, "vasya", 4.8};
    Employee secondEmployee = {2, "petya", 5.2};
    Employee thirdEmployee = {3, "kostya", 7};
    Employee newSecondEmployee = {4, "vitya", 10};
    Employee newThirdEmployee = {5, "sasha", 12};
    std::vector<Employee> employees = {firstEmployee, secondEmployee, thirdEmployee};
    const std::string binaryFileName = "testbinary";
    Model model(binaryFileName);
    model.writeBinaryFile(employees);
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
    std::vector<int> array = {1, 2, 3, 4};
    EXPECT_FALSE(Utility::hasEquals(array));
}

TEST(Utility_hasEquals, ZeroElems)
{
    std::vector<int> array;
    EXPECT_FALSE(Utility::hasEquals(array));
}

TEST(Utility_hasEquals, OneElem)
{
    std::vector<int> array = {1};
    EXPECT_FALSE(Utility::hasEquals(array));
}

TEST(Utility_hasEquals, TwoEquals)
{
    std::vector<int> array = {1, 2, 3, 4, 1};
    EXPECT_TRUE(Utility::hasEquals(array));
}

TEST(Utility_hasEquals, AllEquals)
{
    std::vector<int> array = {1, 1, 1, 1, 1};
    EXPECT_TRUE(Utility::hasEquals(array));
}

TEST(Utility_extractIds, OneElem)
{
    Employee firstEmployee = {1, "vasya", 4.8};
    std::vector<Employee> employees = {firstEmployee};
    std::vector<int> ids = Utility::extractIds(employees);
    for (std::size_t i = 0; i < employees.size(); ++i)
    {
        EXPECT_EQ(ids[i], employees[i].id);
    }
}
TEST(Utility_extractIds, severalElems)
{
    Employee firstEmployee = {1, "vasya", 4.8};
    Employee secondEmployee = {2, "petya", 5.2};
    Employee thirdEmployee = {3, "kostya", 7};
    Employee forthEmployee = {4, "vitya", 10};
    Employee fifthEmployee = {5, "sasha", 12};
    std::vector<Employee> employees = {firstEmployee, secondEmployee, thirdEmployee, forthEmployee, fifthEmployee};
    std::vector<int> ids = Utility::extractIds(employees);
    for (size_t i = 0; i < employees.size(); ++i)
    {
        EXPECT_EQ(ids[i], employees[i].id);
    }
}

TEST(Utility_fillMap, StandartFlow)
{
    Employee firstEmployee = {5, "vasya", 4.8};
    Employee secondEmployee = {1, "petya", 5.2};
    Employee thirdEmployee = {3, "kostya", 7};
    Employee forthEmployee = {2, "vitya", 10};
    Employee fifthEmployee = {4, "sasha", 12};
    std::vector<Employee> employees = {firstEmployee, secondEmployee, thirdEmployee, forthEmployee, fifthEmployee};
    std::map<int, size_t> expectedAnswer;
    expectedAnswer[5] = 0;
    expectedAnswer[1] = 1;
    expectedAnswer[3] = 2;
    expectedAnswer[2] = 3;
    expectedAnswer[4] = 4;
    std::map<int, size_t> filledMap;
    Utility::fillMap(filledMap, employees);
    EXPECT_EQ(expectedAnswer, filledMap);
}

TEST(Controller_Controller, TestDistinctIds)
{
    Employee firstEmployee = {1, "vasya", 4.8};
    Employee secondEmployee = {2, "petya", 5.2};
    Employee thirdEmployee = {3, "kostya", 7};
    Employee forthEmployee = {4, "vitya", 10};
    Employee fifthEmployee = {5, "sasha", 12};
    std::vector<Employee> employees = {firstEmployee, secondEmployee, thirdEmployee, forthEmployee, fifthEmployee};
    const std::string binaryFileName = "testbinary";
    bool ok;
    Controller ctrl(binaryFileName, employees, ok);
    EXPECT_TRUE(ok);
}

TEST(Controller_Controller, TestEqualIds)
{
    Employee firstEmployee = {1, "vasya", 4.8};
    Employee secondEmployee = {2, "petya", 5.2};
    Employee thirdEmployee = {1, "kostya", 7};
    Employee forthEmployee = {4, "vitya", 10};
    Employee fifthEmployee = {5, "sasha", 12};
    std::vector<Employee> employees = {firstEmployee, secondEmployee, thirdEmployee, forthEmployee, fifthEmployee};
    const std::string binaryFileName = "testbinary";
    bool ok;
    Controller ctrl(binaryFileName, employees, ok);
    EXPECT_FALSE(ok);
}

TEST(Controller_getRecord, TestExistingId)
{
    Employee firstEmployee = {1, "vasya", 4.8};
    Employee secondEmployee = {2, "petya", 5.2};
    Employee thirdEmployee = {3, "kostya", 7};
    Employee forthEmployee = {4, "vitya", 10};
    Employee fifthEmployee = {5, "sasha", 12};
    std::vector<Employee> employees = {firstEmployee, secondEmployee, thirdEmployee, forthEmployee, fifthEmployee};
    const std::string binaryFileName = "testbinary";
    bool ok;
    Controller ctrl(binaryFileName, employees, ok);
    EXPECT_TRUE(ok);
    Employee employeeRead;
    EXPECT_TRUE(ctrl.getRecord(1, employeeRead));
    EXPECT_EQ(employeeRead, firstEmployee);
    EXPECT_TRUE(ctrl.getRecord(3, employeeRead));
    EXPECT_EQ(employeeRead, thirdEmployee);
}

TEST(Controller_getRecord, TestFictitiousId)
{
    Employee firstEmployee = {1, "vasya", 4.8};
    Employee secondEmployee = {2, "petya", 5.2};
    Employee thirdEmployee = {3, "kostya", 7};
    Employee forthEmployee = {4, "vitya", 10};
    Employee fifthEmployee = {5, "sasha", 12};
    std::vector<Employee> employees = {firstEmployee, secondEmployee, thirdEmployee, forthEmployee, fifthEmployee};
    const std::string binaryFileName = "testbinary";
    bool ok;
    Controller ctrl(binaryFileName, employees, ok);
    EXPECT_TRUE(ok);
    Employee employeeRead;
    EXPECT_FALSE(ctrl.getRecord(7, employeeRead));
}

TEST(Controller_writeRecord, FictitiousId)
{
    Employee firstEmployee = {1, "vasya", 4.8};
    Employee secondEmployee = {2, "petya", 5.2};
    Employee thirdEmployee = {3, "kostya", 7};
    Employee forthEmployee = {4, "vitya", 10};
    Employee fifthEmployee = {5, "sasha", 12};
    Employee newEmployee = {3, "ilya", 20};
    std::vector<Employee> employees = {firstEmployee, secondEmployee, thirdEmployee, forthEmployee, fifthEmployee};
    const std::string binaryFileName = "testbinary";
    bool ok;
    Controller ctrl(binaryFileName, employees, ok);
    EXPECT_TRUE(ok);

    EXPECT_FALSE(ctrl.setRecord(7, newEmployee));
}

TEST(Controller_writeRecord, SameIdOverwrite)
{
    Employee firstEmployee = {1, "vasya", 4.8};
    Employee secondEmployee = {2, "petya", 5.2};
    Employee thirdEmployee = {3, "kostya", 7};
    Employee forthEmployee = {4, "vitya", 10};
    Employee fifthEmployee = {5, "sasha", 12};
    Employee newEmployee = {3, "ilya", 20};
    std::vector<Employee> employees = {firstEmployee, secondEmployee, thirdEmployee, forthEmployee, fifthEmployee};
    const std::string binaryFileName = "testbinary";
    bool ok;
    Controller ctrl(binaryFileName, employees, ok);
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
    Employee firstEmployee = {1, "vasya", 4.8};
    Employee secondEmployee = {2, "petya", 5.2};
    Employee thirdEmployee = {3, "kostya", 7};
    Employee forthEmployee = {4, "vitya", 10};
    Employee fifthEmployee = {5, "sasha", 12};
    Employee newEmployee = {7, "ilya", 20};
    std::vector<Employee> employees = {firstEmployee, secondEmployee, thirdEmployee, forthEmployee, fifthEmployee};
    const std::string binaryFileName = "testbinary";
    bool ok;
    Controller ctrl(binaryFileName, employees, ok);
    EXPECT_TRUE(ok);
    Employee employeeRead;

    EXPECT_TRUE(ctrl.setRecord(4, newEmployee));
    EXPECT_FALSE(ctrl.getRecord(4, employeeRead));

    EXPECT_TRUE(ctrl.getRecord(7, employeeRead));
    EXPECT_EQ(newEmployee, employeeRead);
}

TEST(Controller_writeRecord, NewIdIsTaken)
{
    Employee firstEmployee = {1, "vasya", 4.8};
    Employee secondEmployee = {2, "petya", 5.2};
    Employee thirdEmployee = {3, "kostya", 7};
    Employee forthEmployee = {4, "vitya", 10};
    Employee fifthEmployee = {5, "sasha", 12};
    Employee newEmployee = {2, "ilya", 20};
    std::vector<Employee> employees = {firstEmployee, secondEmployee, thirdEmployee, forthEmployee, fifthEmployee};
    const std::string binaryFileName = "testbinary";
    bool ok;
    Controller ctrl(binaryFileName, employees, ok);
    EXPECT_TRUE(ok);

    EXPECT_FALSE(ctrl.setRecord(4, newEmployee));
}

TEST(Utility_safeUnsignedIntegerInput, StandardInput)
{
    std::istringstream testStream("1 2 3 4 0\n");
    std::ostringstream ostream;
    size_t a, b, c, d, e;
    a = Utility::safeUnsignedIntegerInput(testStream, ostream, "", "");
    b = Utility::safeUnsignedIntegerInput(testStream, ostream, "", "");
    c = Utility::safeUnsignedIntegerInput(testStream, ostream, "", "");
    d = Utility::safeUnsignedIntegerInput(testStream, ostream, "", "");
    e = Utility::safeUnsignedIntegerInput(testStream, ostream, "", "");
    EXPECT_EQ(a, 1);
    EXPECT_EQ(b, 2);
    EXPECT_EQ(c, 3);
    EXPECT_EQ(d, 4);
    EXPECT_EQ(e, 0);
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
    std::vector<Employee> employees = {firstEmployee, secondEmployee};
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
    std::vector<Employee> employees = {firstEmployee, secondEmployee};
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

    Utility::printEmployees(out, employees);

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
    Employee firstEmployee = {7, "vasya", 4.8};
    Employee secondEmployee = {15, "petya", 5.2};
    Employee thirdEmployee = {2, "kostya", 7};
    Employee forthEmployee = {9, "vitya", 10};
    Employee fifthEmployee = {28, "sasha", 12};
    std::vector<Employee> employees = {firstEmployee, secondEmployee, thirdEmployee, forthEmployee, fifthEmployee};
    const std::string binaryFileName = "testbinary";
    bool ok;
    Controller ctrl(binaryFileName, employees, ok);
    EXPECT_TRUE(ok);

    size_t recordId;
    EXPECT_FALSE(ctrl.idToRecordId(5, recordId));
}

TEST(Controller_idToRecordId, StandartFlow)
{
    Employee firstEmployee = {7, "vasya", 4.8};
    Employee secondEmployee = {15, "petya", 5.2};
    Employee thirdEmployee = {2, "kostya", 7};
    Employee forthEmployee = {9, "vitya", 10};
    Employee fifthEmployee = {28, "sasha", 12};
    std::vector<Employee> employees = {firstEmployee, secondEmployee, thirdEmployee, forthEmployee, fifthEmployee};
    const std::string binaryFileName = "testbinary";
    bool ok;
    Controller ctrl(binaryFileName, employees, ok);
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
    Employee firstEmployee = {7, "vasya", 4.8};
    Employee secondEmployee = {15, "petya", 5.2};
    Employee thirdEmployee = {2, "kostya", 7};
    Employee forthEmployee = {9, "vitya", 10};
    Employee fifthEmployee = {28, "sasha", 12};
    std::vector<Employee> employees = {thirdEmployee, firstEmployee, forthEmployee, secondEmployee, fifthEmployee};
    const std::string binaryFileName = "testbinary";
    bool ok;
    Controller ctrl(binaryFileName, employees, ok);
    EXPECT_TRUE(ok);

    std::vector<Employee> returnedEmployees = ctrl.getAllRecords();

    EXPECT_EQ(employees, returnedEmployees);
}

TEST(ClientHandlerArgs_ClientHandlerArgs, StandartFlow)
{
    Employee firstEmployee = {7, "vasya", 4.8};
    Employee secondEmployee = {15, "petya", 5.2};
    Employee thirdEmployee = {2, "kostya", 7};
    Employee forthEmployee = {9, "vitya", 10};
    Employee fifthEmployee = {28, "sasha", 12};
    std::vector<Employee> employees = {thirdEmployee, firstEmployee, forthEmployee, secondEmployee, fifthEmployee};

    const std::string binaryFileName = "testbinary";

    bool ok;
    std::shared_ptr<Controller> ctrl = std::make_shared<Controller>(binaryFileName, employees, ok);
    EXPECT_TRUE(ok);

    std::size_t id = 1;
    std::size_t numberOfClients = 5;
    std::size_t numberOfRecords = 7;

    std::shared_ptr<CRITICAL_SECTION> iocs = std::make_shared<CRITICAL_SECTION>();
    std::shared_ptr<CRITICAL_SECTION> acs = std::make_shared<CRITICAL_SECTION>();
    std::shared_ptr<std::vector<std::size_t>> recordAccessReadCount = std::make_shared<std::vector<std::size_t>>(numberOfRecords, 0);

    InitializeCriticalSection(iocs.get());

    ClientHandlerArgs args(id, numberOfClients, numberOfRecords, recordAccessReadCount, ctrl, iocs, acs);
    EXPECT_EQ(args.getId(), id);
    EXPECT_EQ(args.getNumberOfClients(), numberOfClients);
    EXPECT_EQ(args.getNumberOfRecords(), numberOfRecords);
    EXPECT_EQ(args.getController(), ctrl);
    EXPECT_EQ(args.getIOCriticalSection(), iocs);
    EXPECT_EQ(args.getArrayCriticalSection(), acs);
    EXPECT_EQ(args.getRecordAccessReadCount(), recordAccessReadCount);
}

TEST(Utility_getAccessMutexName, StandartFlow)
{
    std::string expectedEventName = "Record access mutex 5";
    EXPECT_EQ(expectedEventName, Utility::getAccessMutexName(5));
}

TEST(ClientHandlerArgs, DefaultConstructor)
{
    ClientHandlerArgs args;
    EXPECT_EQ(args.getId(), 0);
    EXPECT_EQ(args.getNumberOfClients(), 0);
    EXPECT_EQ(args.getNumberOfRecords(), 0);
}