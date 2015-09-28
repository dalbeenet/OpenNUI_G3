#include <kernel/session.h>
#include <kernel/device_manager.h>
#include <atomic>

namespace kernel {

void session::_on_message_received(::vee::system::operation_result& result,
                                   ::vee::byte* const raw_data,
                                   uint32_t buf_capacity,
                                   uint32_t byte_transferred)
{
    if (result.error != ::vee::system::error_code::success)
    {
        printf("Message stream is disconnected by client! (sid: %d)\n", get_id());
        return;
    }

    packet_buffer_t pure_data;
    pure_data.fill(0);
    protocol::data_frame_header header = protocol::packet_parsing(raw_data, pure_data.data());
    //TODO: <KNOWN_ISSUE> C# 라이브러리에서 잘못된 ID를을 보내고 있음. 라이브러리 패치 후 서버도 패치해야함
    /*if (header.identifier != stream_constant::opennui_packet_identifier)
    {
    throw vee::exception("Protocol mismatch: invalid identifier", (int)error_code::handshake_failure);
    }*/
    switch (header.opcode)
    {
    case protocol::opcode::cts_request_color_frame:
        printf("session %d> cts_request_color_frame\n", get_id());
        break;
    case protocol::opcode::cts_request_depth_frame:
        printf("session %d> cts_request_depth_frame\n", get_id());
        break;
    case protocol::opcode::cts_request_body_frame:
        printf("session %d> cts_request_body_frame\n", get_id());
        break;
    default:
        printf("Invalid request");
        break;
    }
    get_cts_stream()->async_read(_cts_stream_in_buffer.data(),
                                 _cts_stream_in_buffer.size(),
                                 ::std::bind(&session::_on_message_received, this, ::std::placeholders::_1, ::std::placeholders::_2, ::std::placeholders::_3, ::std::placeholders::_4));

}

uint32_t session::make_unique_sid()
{
    static std::atomic_uint32_t counter(0);
    return ++counter;
}

void session::_launch_api_service()
{
    auto device_mgr = device_manager::get_instance();
    auto device_keys = device_mgr->get_all_keys();
    try
    {
        for (auto& it : device_keys)
        {
            ::std::array<unsigned char, protocol::stream_constant::opennui_packet_maxlen> buffer;
            buffer.fill(0);
            uint32_t packet_size = protocol::utility::packet_generator::stc_new_sensor_online(buffer.data(), it);
            get_stc_stream()->write(buffer.data(), packet_size);
        }
        get_cts_stream()->async_read(_cts_stream_in_buffer.data(),
                                     _cts_stream_in_buffer.size(),
                                     ::std::bind(&session::_on_message_received, this, ::std::placeholders::_1, ::std::placeholders::_2, ::std::placeholders::_3, ::std::placeholders::_4));
    }
    catch (::vee::exception& e)
    {
        printf("Failed to launch api service! [sid: %d]\nUnhandled exception: %s\n", get_id(), e.what());
    }
}

} // namespace kernel