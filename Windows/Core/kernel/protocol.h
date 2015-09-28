#ifndef _OPENNUIG3_KERNEL_PROTOCOL_H_
#define _OPENNUIG3_KERNEL_PROTOCOL_H_

#include <array>
#include <cinttypes>

namespace kernel {
namespace protocol {

using device_key_t = uint32_t;
using session_key_t = uint32_t;
using shared_buffer_key_t = uint32_t;

enum class frame_type: uint32_t
{
    undefined = 0,
    color_frame,
    depth_frame,
    body_frame,
};

struct stream_constant
{
    static const int life_socket_port = 8000;
    static const int opennui_packet_identifier = 0xFD42;
    static const int opennui_packet_maxlen = 1024;
    static const int pipe_in_buffer_capacity = 4096;
    static const int pipe_out_buffer_capacity = 4096;
    static const int shm_lock_block_size = 4;
    static const int shm_buffering_count = 3;
    static const int shm_lock_section_size = shm_lock_block_size * shm_buffering_count;
    static const int shm_time_stamp_block_size = 8;
    static const int temp_g3_body_frame_size = 8196 * 2;
    static const int temp_g3_color_max_frame_size = 1920 * 1080 * 4;
    static const int temp_g3_depth_max_frame_size = 640 * 480 * 2;
    static const int shm_lock_state = 1;
    static const int shm_idle_state = 0;
};

enum class opcode: int16_t
{
    undefined = -1,

    stc_request_platform_type = 0x0001,
    stc_request_pipe_connection = 0x0002,
    stc_request_websocket_connection = 0x0002,
    stc_test_textmessage = 0x0003,
    stc_response_color_frame = 0x0004,
    stc_response_depth_frame = 0x0005,
    stc_response_body_frame = 0x0006,
    stc_send_all_sensor_info = 0x0007,
    stc_send_new_sensor_info = 0x0008,
    stc_send_changed_sensor_info = 0x0009,
    stc_send_trigger_event_data = 0x0010,

    cts_hello_ack = 0x0001,
    cts_ready_connection_ack = 0x0002,
    cts_test_textmessage = 0x0003,
    cts_request_color_frame = 0x0004,
    cts_request_depth_frame = 0x0005,
    cts_request_body_frame = 0x0006,
    cts_request_sensor_data = 0x0007,
};

enum class platform: uint32_t
{
    win32 = 0,
    web = 1,
};

struct data_frame_header
{
    static const int header_size = 18;
    uint16_t identifier = stream_constant::opennui_packet_identifier;
    uint32_t data_size = 0;
    opcode   opcode = opcode::undefined;
    ::std::array<unsigned char, 10> padding;
    ::std::array<unsigned char, header_size> to_buffer() const;
    void to_buffer(unsigned char* dest) const;
    void from_raw_data(unsigned char* raw_data);
};

// Returns formatted packet size
uint32_t packet_formatting(unsigned char* dst, data_frame_header& header, const unsigned char* data, uint32_t data_length);
// Returns parsed data frame header
data_frame_header packet_parsing(unsigned char* raw_data, unsigned char* data_buffer);

namespace utility {

struct packet_generator abstract
{
    static uint32_t stc_new_sensor_online(unsigned char* out_buffer, device_key_t key);
    static uint32_t stc_response_shb_request(protocol::frame_type data_type, unsigned char* out_buffer, device_key_t key, bool is_buffer_created, const char* shb_name);
};

} // namespace utility

} // namespace protocol
}

#endif // !_OPENNUIG3_KERNEL_PROTOCOL_H_