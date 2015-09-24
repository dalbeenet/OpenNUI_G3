#ifndef _VEE_VOOST_NET_H_
#define _VEE_VOOST_NET_H_

#include <vee/macro.h>
#include <vee/stream.h>
#include <vee/exception.h>
#include <memory>

namespace vee {
namespace voost {
namespace net {

using port_t = unsigned short;
using byte = unsigned char;
using operation_result = ::vee::system::operation_result;
using error_code = ::vee::system::error_code;

#define _vee_net_async_connect_callback_sig void(::vee::system::operation_result&)
#define _vee_net_async_read_callback_sig _vee_async_read_callback_sig
#define _vee_net_async_write_callback_sig _vee_async_write_callback_sig
class net_stream abstract: public sync_stream, public async_stream
{
public:
    using async_connect_delegate = delegate<_vee_net_async_connect_callback_sig>;
    using async_connect_delegate_ptr = ::std::shared_ptr<async_connect_delegate>;
    using async_connect_callback = ::std::function<_vee_net_async_connect_callback_sig>;
    virtual ~net_stream() = default;
    virtual void        connect(const char* ip_addr, port_t port) throw(...) = 0;
    virtual void        async_connect(const char* ip_addr, port_t port, async_connect_delegate& e);
    virtual void        async_connect(const char* ip_addr, port_t port, async_connect_delegate&& e);
    virtual void        async_connect(const char* ip_addr, port_t port, async_connect_delegate_ptr& e_ptr);
    virtual void        async_connect(const char* ip_addr, port_t port, async_connect_delegate_ptr&& e_ptr);
    virtual void        async_connect(const char* ip_addr, port_t port, async_connect_callback callback) = 0;
    virtual void        disconnect() = 0;
};

#define _vee_net_async_accept_callback_sig void(::vee::system::operation_result& /*function_result*/, ::std::shared_ptr<::vee::voost::net::net_stream> /*stream*/)
class net_server abstract
{
public:
    using async_accept_delegate_t = delegate<_vee_net_async_accept_callback_sig>;
    virtual ~net_server() = default;
    virtual ::std::shared_ptr<net_stream> accept() throw(...) = 0;
    virtual void async_accept(async_accept_delegate_t e);
    virtual void async_accept(std::shared_ptr<async_accept_delegate_t> e);
    virtual void async_accept(std::function<_vee_net_async_accept_callback_sig> e) = 0;
    virtual void close() = 0;
};

namespace tcp {

::std::shared_ptr<net_server> create_server(unsigned short port);
::std::shared_ptr<net_stream> create_stream();

} // namespace tcp

namespace udp {

} // namespace udp

namespace websocket {

enum class opcode_id: unsigned char
{
    undefined = 0,
    continuation_frame,
    text_frame,
    binary_frame,
    connnection_close,
    ping,
    pong,
    reserved_for_further
};

class websocket_server;
class websocket_stream;

class websocket_stream abstract: public net_stream
{
public:
    struct io_result
    {
        uint32_t header_size = 0;
        uint32_t payload_size = 0;
    };
    virtual ~websocket_stream() = default;
    virtual void        connect(const char* ip_addr, port_t port) throw(...) = 0;
    virtual void        async_connect(const char* ip_addr, port_t port, async_connect_callback e) = 0;
    virtual void        disconnect() = 0;
    virtual uint32_t    write(const byte* data, const uint32_t len) throw(...) override;
    virtual io_result   write(opcode_id opcode, const byte* data, const uint32_t len) throw(...) = 0;
    virtual void        async_write(const byte* data, const uint32_t len, async_write_callback e) throw(...) override;
    virtual void        async_write(opcode_id opcode, const byte* data, const uint32_t len, async_write_delegate& e) throw(...);
    virtual void        async_write(opcode_id opcode, const byte* data, const uint32_t len, async_write_delegate&& e) throw(...);
    virtual void        async_write(opcode_id opcode, const byte* data, const uint32_t len, async_write_delegate_ptr& e_ptr) throw(...);
    virtual void        async_write(opcode_id opcode, const byte* data, const uint32_t len, async_write_delegate_ptr&& e_ptr) throw(...);
    virtual void        async_write(opcode_id opcode, const byte* data, const uint32_t len, async_write_callback e) throw(...) = 0;
    virtual uint32_t    read(byte* const buffer, const uint32_t buf_capacity) throw(...) override;
    virtual io_result   read_payload_only(byte* const buffer, const uint32_t buf_capacity) throw(...) = 0;
    virtual io_result   read_all(byte* const buffer, const uint32_t buf_capacity) throw(...) = 0;
    virtual void        async_read(byte* const buffer, const uint32_t buf_capacity, async_read_callback e) throw(...) override;
    virtual void        async_read_payload_only(byte* const buffer, const uint32_t buf_capacity, async_read_delegate& e) throw(...);
    virtual void        async_read_payload_only(byte* const buffer, const uint32_t buf_capacity, async_read_delegate&& e) throw(...);
    virtual void        async_read_payload_only(byte* const buffer, const uint32_t buf_capacity, async_read_delegate_ptr& e_ptr) throw(...);
    virtual void        async_read_payload_only(byte* const buffer, const uint32_t buf_capacity, async_read_delegate_ptr&& e_ptr) throw(...);
    virtual void        async_read_payload_only(byte* const buffer, const uint32_t buf_capacity, async_read_callback e) throw(...) = 0;
    virtual void        async_read_all(byte* const buffer, const uint32_t buf_capacity, async_read_delegate& e) throw(...);
    virtual void        async_read_all(byte* const buffer, const uint32_t buf_capacity, async_read_delegate&& e) throw(...);
    virtual void        async_read_all(byte* const buffer, const uint32_t buf_capacity, async_read_delegate_ptr& e_ptr) throw(...);
    virtual void        async_read_all(byte* const buffer, const uint32_t buf_capacity, async_read_delegate_ptr&& e_ptr) throw(...);
    virtual void        async_read_all(byte* const buffer, const uint32_t buf_capacity, async_read_callback e) throw(...) = 0;
};

::std::shared_ptr<net_server> create_server(unsigned short port);
::std::shared_ptr<websocket_stream> create_stream();

} // namespace ws

} // namespace net
} // namespace voost
} // namespace vee

#endif // !_VEE_VOOST_NET_H_
