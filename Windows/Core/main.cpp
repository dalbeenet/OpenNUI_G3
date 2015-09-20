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
        using vee::voost::net::websocket::ws_stream;
        using vee::voost::net::websocket::opcode_id;
        auto server = vee::voost::net::websocket::create_server(1992);
        auto session1 = std::static_pointer_cast<ws_stream>(server->accept());
        std::array<char, 512> buffer;
        try
        {
            // Welcome message
            char welcome_message[] = "Welcome";
            session1->write(opcode_id::text_frame, welcome_message, strlen(welcome_message));

            // Connect to websocket echo server
            auto session2 = vee::voost::net::websocket::create_stream();
            session2->connect("", 1000);

            while (true)
            {
                auto bytes_transferred = session1->read(buffer.data(), buffer.size());
                printf("s1 recieved: %lldbytes: %s\n", bytes_transferred, buffer.data());
                bytes_transferred = (session2->write(opcode_id::text_frame, buffer.data(), bytes_transferred)).payload_size;
                printf("s2 send: %lldbytes: %s\n", bytes_transferred, buffer.data());
                session2->read(buffer.data(), buffer.size());
                printf("s2 recieved: %lldbytes: %s\n", bytes_transferred, buffer.data());
                bytes_transferred = (session1->write(opcode_id::text_frame, buffer.data(), bytes_transferred)).payload_size;
                printf("s1 send: %lldbytes: %s\n", bytes_transferred, buffer.data());
            }
        }
        catch (vee::exception& e)
        {
            printf("EXCEPTION: %s\n", e.what());
        }
        print_line();
    }

    return 0;
}