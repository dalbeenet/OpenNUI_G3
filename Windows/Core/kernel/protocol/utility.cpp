#include <kernel/protocol.h>
#include <kernel/device_manager.h>
#include <kernel/shared_buffer.h>

namespace kernel {
namespace protocol {
namespace utility {

uint32_t packet_generator::stc_new_sensor_online(unsigned char* out_buffer, device_key_t key)
{
    auto device_manager = device_manager::get_instance();
    auto module = device_manager->get_module_pointer(key);
    auto device = module->opennui_device_instance();
    
    // Create temporary buffer
    ::std::array<unsigned char, stream_constant::opennui_packet_maxlen> buffer;
    buffer.fill(0);

    // Setting data section
    uint32_t shift = 0;

    // Unique key
    {
        uint32_t device_key = key;
        memmove(buffer.data() + shift, &device_key, sizeof(uint32_t));
        shift += sizeof(uint32_t);
    }
    // Name
    {
        ::std::string device_name; 
        device->name(device_name);
       uint32_t name_length = device_name.length();
       memmove(buffer.data() + shift, &name_length, sizeof(uint32_t));
       shift += sizeof(uint32_t);
       memmove(buffer.data() + shift, device_name.data(), name_length);
       shift += name_length;
    }
    // Vendor
    {
        ::std::string device_vendor;
        device->vendor(device_vendor);
        uint32_t vendor_length = device_vendor.length();
        memmove(buffer.data() + shift, &vendor_length, sizeof(uint32_t));
        shift += sizeof(uint32_t);
        memmove(buffer.data() + shift, device_vendor.data(), vendor_length);
        shift += vendor_length;
    }
    // State
    {
        uint32_t state = (uint32_t)_OPENNUI opennui_device::state_type::opened;
        memmove(buffer.data() + shift, &state, sizeof(uint32_t));
        shift += sizeof(uint32_t);
    }
    // Color frame
    {
        _OPENNUI video_frame_info info;
        device->get_color_frame_info(info);
        // Color width
        {
            uint32_t width = info.width;
            memmove(buffer.data() + shift, &width, sizeof(uint32_t));
            shift += sizeof(uint32_t);
        }
        // Color height
        {
            uint32_t height = info.height;
            memmove(buffer.data() + shift, &height, sizeof(uint32_t));
            shift += sizeof(uint32_t);
        }
        // Color bpp
        {
            uint32_t bpp = info.byte_per_pixel;
            memmove(buffer.data() + shift, &bpp, sizeof(uint32_t));
            shift += sizeof(uint32_t);
        }
    }
    // Depth frame
    {
        _OPENNUI video_frame_info info;
        device->get_depth_frame_info(info);
        // Depth width
        {
            uint32_t width = info.width;
            memmove(buffer.data() + shift, &width, sizeof(uint32_t));
            shift += sizeof(uint32_t);
        }
        // Depth height
        {
            uint32_t height = info.height;
            memmove(buffer.data() + shift, &height, sizeof(uint32_t));
            shift += sizeof(uint32_t);
        }
        // Depth bpp
        {
            uint32_t bpp = info.byte_per_pixel;
            memmove(buffer.data() + shift, &bpp, sizeof(uint32_t));
            shift += sizeof(uint32_t);
        }
    }
    // Body tracking info
    {
        _OPENNUI body_tracking_info info;
        device->get_body_tracking_info(info);
        // Maximum tracking count
        {
            uint32_t maximum_tracking_count = info.maximum_tracking_bodies;
            memmove(buffer.data() + shift, &maximum_tracking_count, sizeof(uint32_t));
            shift += sizeof(uint32_t);
        }
    }

    // Make packet
    ::std::array<unsigned char, stream_constant::opennui_packet_maxlen> packet_data;
    packet_data.fill(0);

    data_frame_header header;
    header.opcode = protocol::opcode::stc_send_new_sensor_info;
    uint32_t packet_size = protocol::packet_formatting(packet_data.data(), header, buffer.data(), shift);

    memmove(out_buffer, packet_data.data(), packet_size);
    return packet_size;
}

uint32_t packet_generator::stc_response_shb_request(protocol::frame_type data_type, unsigned char* out_buffer, device_key_t key, bool is_buffer_created, const char* shb_name)
{
    auto device_manager = device_manager::get_instance();
    auto module = device_manager->get_module_pointer(key);
    auto device = module->opennui_device_instance();

    // Create temporary buffer
    ::std::array<unsigned char, stream_constant::opennui_packet_maxlen> buffer;
    buffer.fill(0);

    // Setting data section
    uint32_t shift = 0;

    // Unique key
    {
        uint32_t device_key = key;
        memmove(buffer.data() + shift, &device_key, sizeof(uint32_t));
        shift += sizeof(uint32_t);
    }
    // Boolean flag
    {
        bool shm_created = is_buffer_created;
        memmove(buffer.data() + shift, &is_buffer_created, sizeof(bool));
        shift += sizeof(bool);
    }
    // Shared buffer name
    if (is_buffer_created)
    {
        uint32_t name_length = strlen(shb_name);
        memmove(buffer.data() + shift, &name_length, sizeof(uint32_t));
        shift += sizeof(uint32_t);
        memmove(buffer.data() + shift, shb_name, name_length);
        shift += name_length;
    }
    else
    {
        char null_name[] = "null";
        uint32_t name_length = strlen(null_name);
        memmove(buffer.data() + shift, &name_length, sizeof(uint32_t));
        shift += sizeof(uint32_t);
        memmove(buffer.data() + shift, null_name, name_length);
        shift += name_length;
    }

    // Make packet
    ::std::array<unsigned char, stream_constant::opennui_packet_maxlen> packet_data;
    packet_data.fill(0);

    data_frame_header header;
    switch (data_type)
    {
    case kernel::protocol::frame_type::color_frame:
        header.opcode = protocol::opcode::stc_response_color_frame;
        break;
    case kernel::protocol::frame_type::depth_frame:
        header.opcode = protocol::opcode::stc_response_depth_frame;
        break;
    case kernel::protocol::frame_type::body_frame:
        header.opcode = protocol::opcode::stc_response_body_frame;
        break;
    case kernel::protocol::frame_type::undefined:
    default:
        header.opcode = protocol::opcode::undefined;
        break;
    }
    uint32_t packet_size = protocol::packet_formatting(packet_data.data(), header, buffer.data(), shift);

    memmove(out_buffer, packet_data.data(), packet_size);
    return packet_size;
}

} // namespace utility
} // namespace protocol
} // namespace kernel