//
// Created by Nikita Kasinski
//

#include <string>
#include <iostream>
#include "client_handler.h"
#include "args.h"
#include "controller.h"
#include "utility.h"
#include "protocol.h"
#include "concurrent_writer.h"
#include "smart_winapi.h"
#include "menu/server_menu.h"

DWORD WINAPI ClientHandler(LPVOID _args)
{
    const std::string pipeName = R"(\\.\pipe\os-lab5-pipe)";

    // processing passed arguments
    ClientHandlerArgs args = *(ClientHandlerArgs *)_args;
    std::size_t numberOfClients = args.getNumberOfClients();
    std::size_t threadId = args.getId();
    std::shared_ptr<std::vector<std::size_t>> recordAccessReadCount = args.getRecordAccessReadCount();
    std::shared_ptr<Controller> ctrl = args.getController();
    std::shared_ptr<CRITICAL_SECTION> iocs = args.getIOCriticalSection();
    std::shared_ptr<CRITICAL_SECTION> acs = args.getArrayCriticalSection();

    ConcurrentWriter writer(iocs, std::cout);

    // creating named pipe
    std::shared_ptr<HANDLE> pipe = SmartWinapi::make_shared_handle(CreateNamedPipeA(
        pipeName.c_str(),
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        numberOfClients,
        0,
        0,
        INFINITE,
        NULL));

    if (SmartWinapi::unwrap(pipe) == INVALID_HANDLE_VALUE)
    {
        // pipe didn't open
        writer.write("Thread ", threadId, " cannot open a pipe: ", GetLastError(), "\n");
        return 2;
    }

    if (!ConnectNamedPipe(SmartWinapi::unwrap(pipe), NULL))
    {
        writer.write("Thread ", threadId, ". Connection error: ", GetLastError(), "\n");
        return 3;
    }

    ServerMenu menu(pipe, threadId, ctrl, recordAccessReadCount, iocs, acs);
    ResultCode result = menu.start();
    if (ResultCode::OK != result)
    {
        writer.write("Thread ", threadId, ": execution error ", result, "\n");
    }

    writer.write("Thread ", threadId, " ended\n");

    return 0;
}