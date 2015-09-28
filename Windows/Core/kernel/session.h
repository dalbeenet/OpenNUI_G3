#ifndef _OPENNUIG3_KERNEL_SESSION_H_
#define _OPENNUIG3_KERNEL_SESSION_H_
#include <vee/voost/net.h>
#include <kernel/protocol.h>

namespace kernel {

class session abstract
{
    friend class session_manager;
public:
    using  life_stream = ::std::shared_ptr<::vee::voost::net::net_stream>;
    using  session_id_t = uint32_t;
    static session_id_t make_unique_sid();
    virtual ~session() = default;
    virtual session_id_t get_id() const = 0;
    virtual life_stream  get_life_stream() const = 0;
protected:
    using packet_buffer_t = ::std::array<unsigned char, protocol::stream_constant::opennui_packet_maxlen>;
    packet_buffer_t _lifestream_in_buffer;
    packet_buffer_t _cts_stream_in_buffer;
};
using session_ptr = ::std::shared_ptr<session>;

} // namespace kernel

#endif // _OPENNUIG3_KERNEL_SESSION_H_