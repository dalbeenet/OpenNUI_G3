#include <kernel/session/win32_session.h>
#include <thread>
#include <future>
#pragma warning(disable:4996)
namespace kernel {

::std::shared_ptr<win32_session> win32_session::handshake(life_stream raw_stream, session_id_t sid) throw(...)
{
    ::std::array<unsigned char, 512> pipe_name;
    pipe_name.fill(0);
    sprintf((char*)(pipe_name.data()), "opennui_pipe_%d", sid);
    
    auto stream_pair = _message_stream_connection(raw_stream, (char*)pipe_name.data());
    win32_message_stream& cts_stream = stream_pair.first;
    win32_message_stream& stc_stream = stream_pair.second;
    
    ::std::shared_ptr<win32_session> session = ::std::make_shared<win32_session>();
    session->_life_stream = std::move(raw_stream);
    session->_cts_stream  = std::move(cts_stream);
    session->_stc_stream  = std::move(stc_stream);
    session->_session_id  = sid;
    return session;
}

win32_session::session_id_t win32_session::get_id() const
{
    return _session_id;
}

win32_session::life_stream win32_session::get_life_stream()
{
    return _life_stream;
}

win32_session::message_stream win32_session::get_cts_stream()
{
    return ::std::static_pointer_cast<::vee::iostream>(_cts_stream);
}

win32_session::message_stream win32_session::get_stc_stream()
{
    return ::std::static_pointer_cast<::vee::iostream>(_stc_stream);
}

win32_session::win32_message_stream win32_session::get_cts_stream_native()
{
    return _cts_stream;
}

win32_session::win32_message_stream win32_session::get_stc_stream_native()
{
    return _stc_stream;
}

protocol::platform win32_session::get_platform() const
{
    return protocol::platform::win32;
}

::std::pair<win32_session::win32_message_stream/*CTS*/, win32_session::win32_message_stream/*STC*/> win32_session::_message_stream_connection(life_stream raw_stream, const char* pipe_name) throw(...)
{
    using ::kernel::protocol::stream_constant;
    using ::vee::voost::interprocess::pipe_data_transfer_mode;
    using ::vee::voost::interprocess::pipe_access_mode;
    ::std::pair<win32_message_stream/*CTS*/, win32_message_stream/*STC*/> stream_pair;
    win32_message_stream& cts_stream = stream_pair.first;
    win32_message_stream& stc_stream = stream_pair.second;
    // Create a pipe stream server
    auto cts_pipe_server = vee::voost::interprocess::win32::create_named_pipe_server();
    auto stc_pipe_server = vee::voost::interprocess::win32::create_named_pipe_server();
    {
        // Async task
        auto cts_connection = std::async(std::launch::async, [&] // CTS connection
        {
            std::string cts_pipe_name(pipe_name);
            cts_pipe_name.append("_cts");
            cts_stream = cts_pipe_server->accept(cts_pipe_name.data(), pipe_access_mode::inbound, pipe_data_transfer_mode::iomode_message, stream_constant::pipe_in_buffer_capacity, stream_constant::pipe_out_buffer_capacity);
        });
        auto stc_connection = std::async(std::launch::async, [&] // STC connection
        {
            std::string stc_pipe_name(pipe_name);
            stc_pipe_name.append("_stc");
            stc_stream = cts_pipe_server->accept(stc_pipe_name.data(), pipe_access_mode::outbound, pipe_data_transfer_mode::iomode_message, stream_constant::pipe_in_buffer_capacity, stream_constant::pipe_out_buffer_capacity);
        });

        protocol::data_frame_header header;
        header.opcode = protocol::opcode::stc_request_pipe_connection;
        ::std::array<unsigned char, protocol::stream_constant::opennui_packet_maxlen> temp_buffer, packet_buffer;
        temp_buffer.fill(0);
        packet_buffer.fill(0);
        uint32_t data_length = sizeof(uint32_t)/*STRING_LEN*/ + strlen(pipe_name);
        memmove(temp_buffer.data(), &data_length, sizeof(uint32_t));
        *((int*)temp_buffer.data()) = strlen(pipe_name);
        sprintf((char*)temp_buffer.data() + sizeof(uint32_t), (0, "%s"), pipe_name);
        uint32_t packet_size = protocol::packet_formatting(packet_buffer.data(), header, temp_buffer.data(), data_length);
        raw_stream->write(packet_buffer.data(), packet_size);

        cts_connection.get();
        stc_connection.get();
        // Break a this block when async task finished.
    }
    // Receive to READY_CONNECTION_ACK from client
    {
        ::std::array<unsigned char, protocol::stream_constant::opennui_packet_maxlen> raw_data;
        cts_stream->read(raw_data.data(), raw_data.size());
        ::std::array<unsigned char, protocol::stream_constant::opennui_packet_maxlen> pure_data;
        pure_data.fill(0);
        protocol::data_frame_header header = protocol::packet_parsing(raw_data.data(), pure_data.data());
        //TODO: <KNOWN_ISSUE> C# 라이브러리에서 잘못된 ID를을 보내고 있음. 라이브러리 패치 후 서버도 패치해야함
        /*if (header.identifier != stream_constant::opennui_packet_identifier)
        {
        throw vee::exception("Protocol mismatch: invalid identifier", (int)error_code::handshake_failure);
        }*/
        if (header.opcode != protocol::opcode::cts_ready_connection_ack)
        {
            throw vee::exception("Protocol mismatch: cts_ready_connection_ack error", (int)error_code::handshake_failure);
        }
    }
    return stream_pair;
}

win32_session::~win32_session()
{

}

win32_session::win32_session()
{

}

win32_session::win32_session(win32_session&& other)
{
    
}

} // namespace kernel