/******************************************************************* 
FileName: main.cpp
Author  : vee
Date    : 2015-08-01 22:00:54
Desc    : Entry point of nui framework core
*******************************************************************/ 

#include <iostream>
#include <array>
#include <kernel/kernel.h>
#include <vee/voost/timer.h>
#include <vee/voost/net.h>
#include <conio.h>

void print_line()
{
    for (int i = 0; i < 80; ++i)
        printf("-");
}

void __cdecl f1(void*)
{
    puts(__FUNCSIG__);
}

void __stdcall f2(void*)
{
    puts(__FUNCSIG__);
}

void timer_callback(unsigned int timer_tick)
{
    printf("tick: %d\n", timer_tick);
}

int main()
{
    ////TODO: Scheduler test
    //printf("Scheduler test\n");
    //xkernel::scheduler::initialize();
    //xkernel::scheduler::request(f1, nullptr);
    //xkernel::scheduler::request(f2, nullptr);
    //getch();
    //print_line();
    //xkernel::scheduler::dispose();
    //{
    //    printf("Timer test\npress any key to stop a timer...\n");
    //    auto timer = vee::voost::timer::create_timer();
    //    timer->run(500, vee::make_delegate<void(unsigned int)>(timer_callback));
    //    getch();
    //    print_line();
    //}
    
    {
        printf("Websocket echo server\n");
        auto server = vee::voost::net::websocket::create_server(1992);
        auto session = server->accept();
        std::array<char, 512> buffer;
        try
        {
            char welcome_message[] = "Welcome";
            session->write(welcome_message, strlen(welcome_message));

            while (true)
            {
                auto bytes_transferred = session->read(buffer.data(), buffer.size());
                printf("recv %lldbytes: %s\n", bytes_transferred, buffer.data());
                bytes_transferred = session->write(buffer.data(), bytes_transferred);
                printf("echo %lldbytes: %s\n", bytes_transferred, buffer.data());
            }
        }
        catch (vee::exception& e)
        {
            printf("HELLO EXCEPTION: %s\n", e.what());
        }
        print_line();
    }

    return 0;
}