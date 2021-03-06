/******************************************************************* 
FileName: main.cpp
Author  : vee
Date    : 2015-08-01 22:00:54
Desc    : Entry point of nui framework core
*******************************************************************/ 

#include <kernel/gateway.h>
#include <kernel/forward_unit.h>
#include <iostream>
#include <conio.h>
#include <csignal>
#pragma warning(disable:4996)

void signalHandler(int signum)
{
    std::function<int(int)> f;
    std::cout << "Interrupt signal (" << signum << ") received.\n";

    // cleanup and close up stuff here  
    // terminate program  

    exit(signum);
}

BOOL CtrlHandler(DWORD fdwCtrlType)
{
    switch (fdwCtrlType)
    {
        // Handle the CTRL-C signal. 
    case CTRL_C_EVENT:
        printf("Ctrl-C event\n\n");
        return(TRUE);

        // CTRL-CLOSE: confirm that the user wants to exit. 
    case CTRL_CLOSE_EVENT:
        printf("Ctrl-Close event\n\n");
        return(TRUE);

        // Pass other signals to the next handler. 
    case CTRL_BREAK_EVENT:
        printf("Ctrl-Break event\n\n");
        return FALSE;

    case CTRL_LOGOFF_EVENT:
        printf("Ctrl-Logoff event\n\n");
        return FALSE;

    case CTRL_SHUTDOWN_EVENT:
        printf("Ctrl-Shutdown event\n\n");
        return FALSE;

    default:
        return FALSE;
    }
}

#ifdef _DEBUG
#define KINECT2_MODULE_NAME "opennui_kinect2-x32d.nuimodule"
#define KINECT1_MODULE_NAME "opennui_kinect1-x32d.nuimodule"
#define XTION_PRO_LIVE_MODULE_NAME "opennui_xtion_pro_live-x32d.nuimodule"
#else
#define KINECT2_MODULE_NAME "opennui_kinect2-x32.nuimodule"
#define KINECT1_MODULE_NAME "opennui_kinect1-x32.nuimodule"
#define XTION_PRO_LIVE_MODULE_NAME "opennui_xtion_pro_live-x32.nuimodule"
#endif

int main()
{
    signal(SIGINT, signalHandler);
    signal(SIGABRT, signalHandler);
    signal(SIGTERM, signalHandler);
    signal(SIGFPE, signalHandler);
    if (SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE));

    auto forward_unit = kernel::forward_unit::get_instance();
    auto gateway = kernel::gateway::get_instance();
    auto device_manager = kernel::device_manager::get_instance();
    try
    {
        device_manager->add_module(KINECT2_MODULE_NAME);
    }
    catch (vee::exception& e)
    {
        printf("system> Could not add a module %s to device manager, %s\n", KINECT2_MODULE_NAME, e.what());
    }

    try
    {
        device_manager->add_module(KINECT1_MODULE_NAME);
    }
    catch (vee::exception& e)
    {
        printf("system> Could not add a module %s to device manager, %s\n", KINECT1_MODULE_NAME, e.what());
    }

    try
    {
        device_manager->add_module(XTION_PRO_LIVE_MODULE_NAME);
    }
    catch (vee::exception& e)
    {
        printf("system> Could not add a module %s to device manager, %s\n", XTION_PRO_LIVE_MODULE_NAME, e.what());
    }

    while (true)
    {
        //printf("Press esc key to exit.\r");
        char c = getch();
        if (c == 0x27)
            break;
    }

    //printf("system> press any key to exit...\n");
    //_getch();
    return 0;
}
#if 0 // NAMED_PIPE_TEST_CODe

#include <vee/voost/pipe.h>
void named_pipe_echo_server()
{
    {
        printf("system> Named pipe echo server\n");
        using vee::byte;
        using vee::operation_result;
        using vee::voost::interprocess::creation_option;
        using vee::voost::interprocess::pipe_data_transfer_mode;
        using vee::voost::interprocess::named_pipe;
        using vee::voost::interprocess::named_pipe_server;
        using vee::voost::interprocess::pipe_time_out_constant;

        auto server = vee::voost::interprocess::win32::create_named_pipe_server();
        
        try
        {
            auto session = server->accept("\\\\.\\pipe\\opennuipipe2",
                                          pipe_data_transfer_mode::iomode_message, 1024, 1024);
            printf("system> Client connected.\n");
            std::array<byte, 512> buffer;
            // Welcome message
            char welcome_message[] = "Welcome";
            session->write((byte*)welcome_message, strlen(welcome_message));

            while (true)
            {
                unsigned int bytes_transferred = session->read(buffer.data(), buffer.size());
                printf("system> data recv: %dbytes: %s\n", bytes_transferred, buffer.data());
                //bytes_transferred = session->write((byte*)buffer.data(), bytes_transferred);
                //printf("system> echo send: %dbytes: %s\n", bytes_transferred, buffer.data());
                session->async_write(buffer.data(),
                                     bytes_transferred,
                                     [](operation_result& result, size_t bytes_transferred) -> void
                                     {
                                        printf("system> echo send: %dbytes\n", bytes_transferred);
                                    });
            }
        }
        catch (vee::exception& e)
        {
            printf("system> exception occured!\nerror_code: %d\nmessage: %s\n", e.code, e.desc.data());
        }
    }
}
#endif

#if 0 // WEBSOCKET_TEST_CODE
#include <vee/voost/net.h>
void websocket_echo_server_main()
{
    {
        printf("system> Websocket echo server\n");
        using vee::voost::net::byte;
        using vee::voost::net::net_stream;
        using vee::voost::net::error_code;
        using vee::voost::net::websocket::websocket_stream;
        using vee::voost::net::websocket::opcode_id;
        using vee::system::operation_result;

        auto server = vee::voost::net::websocket::create_server(1992);
        auto session1 = std::static_pointer_cast<websocket_stream>(server->accept());
        std::array<byte, 512> buffer;
        try
        {
            // Welcome message
            char welcome_message[] = "Welcome";
            session1->write(opcode_id::text_frame, (byte*)welcome_message, strlen(welcome_message));

            while (true)
            {
                unsigned int bytes_transferred = session1->read(buffer.data(), buffer.size());
                printf("system> data recv: %dbytes: %s\n", bytes_transferred, buffer.data());
                //bytes_transferred = (session1->write(opcode_id::text_frame, buffer.data(), bytes_transferred)).payload_size;
                //printf("system> echo send: %dbytes: %s\n", bytes_transferred, buffer.data());
                session1->async_write(opcode_id::text_frame,
                                      buffer.data(),
                                      bytes_transferred,
                                      [](operation_result& result, size_t bytes_transferred) -> void
                {
                    printf("system> echo send: %dbytes\n", bytes_transferred);
                });
            }
        }
        catch (vee::exception& e)
        {
            printf("system> exception occured!: %s\n", e.what());
        }
    }
}
#endif

//void scheduler_test_code()
//{
//    printf("system> Scheduler test\n");
//    xkernel::scheduler::initialize();
//    xkernel::scheduler::request(f1, nullptr);
//    xkernel::scheduler::request(f2, nullptr);
//    getch();
//    print_line();
//    xkernel::scheduler::dispose();
//    {
//        printf("system> Timer test\npress any key to stop a timer...\n");
//        auto timer = vee::voost::timer::create_timer();
//        timer->run(500, vee::make_delegate<void(unsigned int)>(timer_callback));
//        getch();
//        print_line();
//    }
//}