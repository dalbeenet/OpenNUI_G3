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
    
    //{
    //    printf("Websocket echo server\n");
    //    using vee::voost::net::byte;
    //    using vee::voost::net::net_stream;
    //    using vee::voost::net::error_code;
    //    using vee::voost::net::websocket::ws_stream;
    //    using vee::voost::net::websocket::opcode_id;
    //    using op_result = vee::voost::net::op_result;

    //    auto server = vee::voost::net::websocket::create_server(1992);
    //    std::function<_vee_net_async_accept_callback_sig> accept_handler =
    //        [&](op_result& result, std::shared_ptr<net_stream> session) -> void
    //    {
    //        if (result.error != error_code::none) return;
    //        auto web_session = std::static_pointer_cast<ws_stream>(session);
    //        std::shared_ptr< std::array<byte, 512> > buffer = std::make_shared< std::array<byte, 512> >();
    //        std::function<_vee_net_async_read_callback_sig> read_handler =
    //            [&read_handler, web_session](op_result& result, byte* const recv_buffer, size_t buf_capacity, size_t bytes_transferred)
    //        {
    //            if (result.error != error_code::none) return;
    //            printf("data recv: %dbytes: %s\n", bytes_transferred, recv_buffer);
    //            web_session->async_write(opcode_id::text_frame,
    //                                     recv_buffer,
    //                                     bytes_transferred,
    //                                     [recv_buffer, buf_capacity, web_session](op_result& result, size_t bytes_transferred) -> void
    //            {
    //                printf("echo send: %dbytes\n", bytes_transferred);
    //                // web_session->async_read(recv_buffer, buf_capacity, read_handler);
    //            });
    //        };
    //        web_session->async_read(buffer->data(), buffer->size(), read_handler);
    //        server->async_accept(accept_handler);
    //    };
    //    server->async_accept(accept_handler);
    //    printf("Press any key to exit...\n");
    //    getch();
    //    print_line();
    //}

    //    auto server = vee::voost::net::websocket::create_server(1992);
    //    auto session1 = std::static_pointer_cast<ws_stream>(server->accept());
    //    std::array<byte, 512> buffer;
    //    try
    //    {
    //        //x Connect to websocket echo server
    //        //x auto session2 = vee::voost::net::websocket::create_stream();
    //        //x session2->connect("175.126.232.80", 12998);
    //        //x session2->connect("174.129.224.73", 80);

    //        // Welcome message
    //        char welcome_message[] = "Welcome";
    //        session1->write(opcode_id::text_frame, (byte*)welcome_message, strlen(welcome_message));

    //        while (true)
    //        {
    //            unsigned int bytes_transferred = session1->read(buffer.data(), buffer.size());
    //            printf("data recv: %dbytes: %s\n", bytes_transferred, buffer.data());
    //            //bytes_transferred = (session1->write(opcode_id::text_frame, buffer.data(), bytes_transferred)).payload_size;
    //            //printf("echo send: %dbytes: %s\n", bytes_transferred, buffer.data());
    //            session1->async_write(opcode_id::text_frame, 
    //                                  buffer.data(), 
    //                                  bytes_transferred, 
    //                                  [](op_result& result, size_t bytes_transferred) -> void
    //            {
    //                printf("echo send: %dbytes\n", bytes_transferred);
    //            });
    //        }
    //    }
    //    catch (vee::exception& e)
    //    {
    //        printf("EXCEPTION: %s\n", e.what());
    //    }
    //}

    return 0;
}