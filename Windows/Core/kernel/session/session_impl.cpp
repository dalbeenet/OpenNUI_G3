#include <kernel/session.h>
#include <kernel/device_manager.h>
#include <atomic>
#pragma warning(disable:4996)
namespace kernel {

void session::_on_message_received(session_ptr this_ptr,
                                   ::vee::system::operation_result& result,
                                   ::vee::byte* const raw_data,
                                   uint32_t buf_capacity,
                                   uint32_t byte_transferred)
{
    if (result.error != ::vee::system::error_code::success)
    {
        printf("system> message stream is disconnected by client! (sid: %d)\n", this_ptr->get_id());
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

    auto parse_device_key = [](unsigned char* pure_data) -> protocol::device_key_t
    {
        protocol::device_key_t key = 0;
        memmove(&key, pure_data, sizeof(protocol::device_key_t));
        return key;
    };

    auto generate_unique_name = [&](const char* inital_string, protocol::device_key_t device_key) -> ::std::string
    {
        ::std::array<char, 1024> buffer;
        buffer.fill(0);
        sprintf(buffer.data(), "%s_did[%d]_sid[%d]", inital_string, device_key, this_ptr->get_id());
        ::std::string s(buffer.data());
        return s;
    };

    auto calculate_shm_size = [](uint32_t frame_size, uint32_t buffering_count) -> uint32_t
    {
        uint32_t lock_region_size = protocol::stream_constant::shm_lock_block_size * buffering_count;
        uint32_t data_unit_size = protocol::stream_constant::shm_lock_block_size + frame_size;
        return lock_region_size + (data_unit_size * buffering_count);
    };

    try
    {
        protocol::device_key_t device_key = parse_device_key(pure_data.data());
        auto device_manager = device_manager::get_instance();
        auto module = device_manager->get_module_pointer(device_key);
        auto device = module->opennui_device_instance();

        if (header.opcode == protocol::opcode::cts_request_color_frame)
        {
            printf("system> session %d requests cts_request_color_frame\n", this_ptr->get_id());
            _OPENNUI video_frame_info info;
            device->get_color_frame_info(info);
            auto shb = shared_buffer::crate(generate_unique_name("color_shared_buffer", device_key).data(),
                                 calculate_shm_size(info.size(), protocol::stream_constant::shm_buffering_count),
                                 device_key, this_ptr->get_id(), protocol::frame_type::color_frame);
            this_ptr->color_buffer_table.insert(shb->key(), shb);
            module->color_buffer_table.insert(shb->key(), shb);
            printf("system> %s is regiestered. shm_key: %u\n", shb->name().data(), shb->key());
            // Response
            {
                ::std::array<unsigned char, protocol::stream_constant::opennui_packet_maxlen> buffer;
                buffer.fill(0);
                uint32_t packet_size = protocol::utility::packet_generator::stc_response_shb_request(protocol::frame_type::color_frame, buffer.data(), device_key, true, shb->name().data());
                this_ptr->get_stc_stream()->async_write(buffer.data(), packet_size, [](::vee::system::operation_result&, uint32_t){});
            }
        }
        else if (header.opcode == protocol::opcode::cts_request_depth_frame)
        {
            printf("system> session %d requests cts_request_depth_frame\n", this_ptr->get_id());
            _OPENNUI video_frame_info info;
            device->get_depth_frame_info(info);
            auto shb = shared_buffer::crate(generate_unique_name("depth_shared_buffer", device_key).data(),
                                            calculate_shm_size(info.size(), protocol::stream_constant::shm_buffering_count),
                                            device_key, this_ptr->get_id(), protocol::frame_type::depth_frame);
            this_ptr->depth_buffer_table.insert(shb->key(), shb);
            module->depth_buffer_table.insert(shb->key(), shb);
            printf("system> %s is regiestered. shm_key: %d\n", shb->name().data(), shb->key());
            // Response
            {
                ::std::array<unsigned char, protocol::stream_constant::opennui_packet_maxlen> buffer;
                buffer.fill(0);
                uint32_t packet_size = protocol::utility::packet_generator::stc_response_shb_request(protocol::frame_type::depth_frame, buffer.data(), device_key, true, shb->name().data());
                this_ptr->get_stc_stream()->async_write(buffer.data(), packet_size, [](::vee::system::operation_result&, uint32_t){});
            }
        }
        else if (header.opcode == protocol::opcode::cts_request_body_frame)
        {
            printf("system> session %d requests cts_request_body_frame\n", this_ptr->get_id());
            auto shb = shared_buffer::crate(generate_unique_name("body_shared_buffer", device_key).data(),
                                            calculate_shm_size(protocol::stream_constant::temp_g3_body_frame_size, protocol::stream_constant::shm_buffering_count),
                                            device_key, this_ptr->get_id(), protocol::frame_type::depth_frame);
            this_ptr->body_buffer_table.insert(shb->key(), shb);
            module->body_buffer_table.insert(shb->key(), shb);
            printf("system> %s is regiestered. shm_key: %d\n", shb->name().data(), shb->key());
            // Response
            {
                ::std::array<unsigned char, protocol::stream_constant::opennui_packet_maxlen> buffer;
                buffer.fill(0);
                uint32_t packet_size = protocol::utility::packet_generator::stc_response_shb_request(protocol::frame_type::body_frame, buffer.data(), device_key, true, shb->name().data());
                this_ptr->get_stc_stream()->async_write(buffer.data(), packet_size, [](::vee::system::operation_result& , uint32_t){});
            }
        }
        else
        {
            printf("system> session %d requests invalid request");
        }
    }
    catch (::vee::exception& e)
    {
        printf("system> failed to procesing api request! [sid: %u]\nUnhandled exception: %s\n", this_ptr->get_id(), e.what());
    }

    this_ptr->get_cts_stream()->async_read(this_ptr->_cts_stream_in_buffer.data(),
                                           this_ptr->_cts_stream_in_buffer.size(),
                                           ::std::bind(&session::_on_message_received, this_ptr, ::std::placeholders::_1, ::std::placeholders::_2, ::std::placeholders::_3, ::std::placeholders::_4));
}

uint32_t session::make_unique_sid()
{
    static std::atomic_uint32_t counter(0);
    return ++counter;
}

void session::_launch_api_service(session_ptr this_ptr)
{
    auto device_mgr = device_manager::get_instance();
    auto device_keys = device_mgr->get_all_keys();
    try
    {
        /*for (auto& it : device_keys)
        {
        ::std::array<unsigned char, protocol::stream_constant::opennui_packet_maxlen> buffer;
        buffer.fill(0);
        uint32_t packet_size = protocol::utility::packet_generator::stc_new_sensor_online(buffer.data(), it);
        this_ptr->get_stc_stream()->async_write(buffer.data(), packet_size, [](::vee::system::operation_result&, uint32_t){});
        }
        this_ptr->get_cts_stream()->async_read(this_ptr->_cts_stream_in_buffer.data(),
        this_ptr->_cts_stream_in_buffer.size(),
        ::std::bind(&session::_on_message_received, this_ptr, ::std::placeholders::_1, ::std::placeholders::_2, ::std::placeholders::_3, ::std::placeholders::_4));*/
        {
            ::std::array<unsigned char, protocol::stream_constant::opennui_packet_maxlen> buffer;
            buffer.fill(0);
            uint32_t packet_size = protocol::utility::packet_generator::stc_intial_online_sensors(buffer.data(), device_keys);
            this_ptr->get_stc_stream()->async_write(buffer.data(), packet_size, [](::vee::system::operation_result&, uint32_t)
            {});
            this_ptr->get_cts_stream()->async_read(this_ptr->_cts_stream_in_buffer.data(),
                                                   this_ptr->_cts_stream_in_buffer.size(),
                                                   ::std::bind(&session::_on_message_received, this_ptr, ::std::placeholders::_1, ::std::placeholders::_2, ::std::placeholders::_3, ::std::placeholders::_4));
        }
    }
    catch (::vee::exception& e)
    {
        printf("system> Failed to launch api service! [sid: %u]\nUnhandled exception: %s\n", this_ptr->get_id(), e.what());
    }
}

} // namespace kernel