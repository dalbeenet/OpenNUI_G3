#ifndef _VEE_VOOST_NET_H_
#define _VEE_VOOST_NET_H_

#include <vee/macro.h>
#include <vee/delegate.h>
#include <vee/exception.h>
#include <memory>

namespace vee {
namespace voost {
namespace net {

using port_t = unsigned short;
using size_t = unsigned int;
using byte = unsigned char;

enum class error_code: int
{
    none = 0,
    connection_failure,
    accept_failure,
    send_failure,
    recv_failure,
    disconnected_by_host,
    rfc6455_client_handshake_failure,
    invalid_connection,
    websocket_heartbeat,
};

struct op_result
{
    error_code  error;
    std::string desc;
};

#define _vee_net_async_connect_callback_sig void(::vee::voost::net::op_result&)
#define _vee_net_async_write_callback_sig void(::vee::voost::net::op_result& /*function_result*/, ::vee::voost::net::size_t /*bytes_transferred*/)
#define _vee_net_async_read_callback_sig void(::vee::voost::net::op_result& /*function_result*/, ::vee::voost::net::byte* const /*recieve_buffer_address*/, ::vee::voost::net::size_t /*recieve_buffer_size*/, ::vee::voost::net::size_t /*bytes_transferred*/)
class net_stream abstract
{
public:
    using async_connect_delegate_t = delegate<_vee_net_async_connect_callback_sig>;
    using async_write_delegate_t = delegate<_vee_net_async_write_callback_sig>;
    using async_read_delegate_t = delegate<_vee_net_async_read_callback_sig>;
    virtual ~net_stream() = default;
    virtual void        connect(const char* ip_addr, port_t port) throw(...) = 0;
    virtual void        async_connect(const char* ip_addr, port_t port, async_connect_delegate_t e);
    virtual void        async_connect(const char* ip_addr, port_t port, std::shared_ptr<async_connect_delegate_t> e);
    virtual void        async_connect(const char* ip_addr, port_t port, std::function<_vee_net_async_connect_callback_sig> callback) = 0;
    virtual void        disconnect() = 0;
    virtual net::size_t write(const byte* data, net::size_t len) throw(...) = 0;
    virtual void        async_write(const byte* data, net::size_t len, async_write_delegate_t e) throw(...);
    virtual void        async_write(const byte* data, net::size_t len, std::shared_ptr<async_write_delegate_t> e) throw(...);
    virtual void        async_write(const byte* data, net::size_t len, std::function<_vee_net_async_write_callback_sig> callback) throw(...) = 0;
    virtual net::size_t read(byte* const buffer, net::size_t buf_capacity) throw(...) = 0;
    virtual void        async_read(byte* const buffer, net::size_t buf_capacity, async_read_delegate_t e) throw(...);
    virtual void        async_read(byte* const buffer, net::size_t buf_capacity, std::shared_ptr<async_read_delegate_t> e) throw(...);
    virtual void        async_read(byte* const buffer, net::size_t buf_capacity, std::function<_vee_net_async_read_callback_sig> callback) throw(...) = 0;
};

#define _vee_net_async_accept_callback_sig void(::vee::voost::net::op_result& /*function_result*/, ::std::shared_ptr<::vee::voost::net::net_stream> /*stream*/)
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

class ws_server;
class ws_stream;

class ws_stream abstract: public net_stream
{
public:
    struct io_result
    {
        net::size_t header_size = 0;
        net::size_t payload_size = 0;
    };
    virtual ~ws_stream() = default;
    virtual void        connect(const char* ip_addr, port_t port) throw(...) = 0;
    //x virtual void        async_connect(const char* ip_addr, port_t port, std::shared_ptr<async_connect_delegate_t> e);
    virtual void        async_connect(const char* ip_addr, port_t port, std::function<_vee_net_async_connect_callback_sig> e) = 0;
    virtual void        disconnect() = 0;
    virtual net::size_t write(const byte* data, net::size_t len) throw(...) override;
    virtual io_result   write(opcode_id opcode, const byte* data, net::size_t len) throw(...) = 0;
    virtual void        async_write(const byte* data, net::size_t len, std::function<_vee_net_async_write_callback_sig> e) throw(...) override;
    virtual void        async_write(opcode_id opcode, const byte* data, net::size_t len, std::shared_ptr<async_write_delegate_t> e) throw(...);
    virtual void        async_write(opcode_id opcode, const byte* data, net::size_t len, std::function<_vee_net_async_write_callback_sig> e) throw(...) = 0;
    virtual net::size_t read(byte* const buffer, net::size_t buf_capacity) throw(...) override;
    virtual io_result   read_payload_only(byte* const buffer, net::size_t buf_capacity) throw(...) = 0;
    virtual io_result   read_all(byte* const buffer, net::size_t buf_capacity) throw(...) = 0;
    //x virtual void        async_read(byte* const buffer, net::size_t buf_capacity, std::shared_ptr<async_read_delegate_t> e) throw(...) override;
    virtual void        async_read(byte* const buffer, net::size_t buf_capacity, std::function<_vee_net_async_read_callback_sig> e) throw(...) override;
    virtual void        async_read_payload_only(byte* const buffer, net::size_t buf_capacity, std::shared_ptr<async_read_delegate_t> e) throw(...);
    virtual void        async_read_payload_only(byte* const buffer, net::size_t buf_capacity, std::function<_vee_net_async_read_callback_sig> e) throw(...) = 0;
    virtual void        async_read_all(byte* const buffer, net::size_t buf_capacity, std::shared_ptr<async_read_delegate_t> e) throw(...);
    virtual void        async_read_all(byte* const buffer, net::size_t buf_capacity, std::function<_vee_net_async_read_callback_sig> e) throw(...) = 0;
};

::std::shared_ptr<net_server> create_server(unsigned short port);
::std::shared_ptr<ws_stream> create_stream();

} // namespace ws

} // namespace net
} // namespace voost
} // namespace vee

#endif // !_VEE_VOOST_NET_H_
