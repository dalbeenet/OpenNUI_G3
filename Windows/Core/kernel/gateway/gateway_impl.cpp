#include <kernel/gateway.h>
#include <kernel/session/win32_session.h>
#include <kernel/session_manager.h>
#include <thread>

namespace kernel {

::std::shared_ptr<gateway> gateway::get_instance()
{
    // The C++11 standard¡¯s got our back in ¡×6.7.4:
    // If control enters the declaration concurrently while the variable is being initialized, 
    // the concurrent execution shall wait for completion of the initialization.
    static ::std::shared_ptr<gateway> instance(new gateway);
    return instance;
}

void gateway::_accept_handler(operation_result& result, net_stream_ptr stream)
{
    if (result.error == net_error_code::success)
    {
        std::thread handshake_thread(_handshake, stream);
        handshake_thread.detach();
        _server->async_accept(::std::bind(&gateway::_accept_handler, this, ::std::placeholders::_1, ::std::placeholders::_2));
    }
    else
    {
        printf("system> The error is occured at the %s\nerror code: %d\nerror desc: %s\n", __FUNCTION__, result.error, result.desc.data());
    }
}

void gateway::_handshake(net_stream_ptr raw_stream)
{
    session::session_id_t sid = session::make_unique_sid();
    // Start handshake process    
    try
    {
        // STC: HELLO
        {
            protocol::data_frame_header header;
            header.opcode = protocol::opcode::stc_request_platform_type;
            ::std::array<unsigned char, 256> buffer, packet_buffer;
            buffer.fill(0);
            packet_buffer.fill(0);
            memmove(buffer.data(), &sid, sizeof(sid));
            uint32_t packet_size = protocol::packet_formatting(packet_buffer.data(), header, buffer.data(), sizeof(sid));
            raw_stream->write(packet_buffer.data(), packet_size);
        }
        // CTS: HELLO_ACK
        protocol::platform platform_code;
        {
            ::std::array<unsigned char, 256> raw_data, pure_data;
            raw_data.fill(0);
            pure_data.fill(0);
            raw_stream->read(raw_data.data(), raw_data.size());
            protocol::data_frame_header header = protocol::packet_parsing(raw_data.data(), pure_data.data());
            memmove(&platform_code, pure_data.data(), sizeof(uint32_t));
        }
        // STC: Data stream connection
        session_ptr new_session;
        switch (platform_code)
        {
        case kernel::protocol::platform::win32:
            new_session = win32_session::handshake(raw_stream, sid);
            break;
        case kernel::protocol::platform::web:
            printf("system> TODO\n");
            break;
        default:
            throw vee::exception("Platform mismatch.", (int)error_code::handshake_failure);
            break;
        }
        auto session_manager = session_manager::get_instance();
        session_manager->add_session(new_session);
    }
    catch (vee::exception& e)
    {
        printf("system> Unhandled exception: %s\n", e.what());
    }
}

gateway::~gateway()
{
    puts(__FUNCTION__);
}

gateway::gateway():
_server(nullptr),
_webserver(nullptr)
{
    puts(__FUNCTION__);
    _server = ::vee::voost::net::tcp::create_server(protocol::stream_constant::win32_life_socket_port);
    _server->async_accept(::std::bind(&gateway::_accept_handler, this, ::std::placeholders::_1, ::std::placeholders::_2));
    _webserver = ::vee::voost::net::websocket::create_server(protocol::stream_constant::web_life_socket_port);
    //TODO: WEB SESSION ACCEPTOR
}

} // namespace 