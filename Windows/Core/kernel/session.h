#ifndef _OPENNUIG3_KERNEL_SESSION_H_
#define _OPENNUIG3_KERNEL_SESSION_H_
#include <vee/voost/net.h>
#include <vee/key_value_table.h>
#include <kernel/shared_buffer.h>

namespace kernel {

class session;
using session_ptr = ::std::shared_ptr<session>;

class session abstract
{
    friend class session_manager;
public:
    using   message_stream = ::std::shared_ptr<::vee::iostream>;
    using   life_stream = ::std::shared_ptr<::vee::voost::net::net_stream>;
    using   session_id_t = uint32_t;
    static  session_id_t make_unique_sid();
    virtual ~session() = default;
    virtual session_id_t get_id() const = 0;
    virtual life_stream  get_life_stream() = 0;
    virtual message_stream get_cts_stream() = 0;
    virtual message_stream get_stc_stream() = 0;
    virtual protocol::platform get_platform() const = 0;
protected:
    static void _launch_api_service(session_ptr this_ptr);
private:
    static void _on_message_received(session_ptr this_ptr,
                              ::vee::system::operation_result& result,
                              ::vee::byte* const raw_data,
                              uint32_t buf_capacity,
                              uint32_t byte_transferred);

public:
    ::vee::key_value_table<protocol::shared_buffer_key_t, shared_buffer_ptr, ::vee::spin_lock> color_buffer_table;
    ::vee::key_value_table<protocol::shared_buffer_key_t, shared_buffer_ptr, ::vee::spin_lock> depth_buffer_table;
    ::vee::key_value_table<protocol::shared_buffer_key_t, shared_buffer_ptr, ::vee::spin_lock> body_buffer_table;
protected:
    using packet_buffer_t = ::std::array<unsigned char, protocol::stream_constant::opennui_packet_maxlen>;
    packet_buffer_t _lifestream_in_buffer;
    packet_buffer_t _cts_stream_in_buffer;
};

} // namespace kernel

#endif // _OPENNUIG3_KERNEL_SESSION_H_