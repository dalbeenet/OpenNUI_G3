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
    send_failure,
    recv_failure,
    disconnected_by_host,
    rfc6455_client_handshake_failure,
    invalid_connection,
    websocket_heartbeat,
};

class net_stream abstract
{
public:
    struct op_result
    {
        error_code  error;
        std::string desc;
    };
    struct io_result
    {
        net::size_t header_size = 0;
        net::size_t payload_size = 0;
    };
    using async_connect_callback = delegate<void(op_result&)>;
    using async_write_callback = delegate<void(op_result& /*function_result*/, net::size_t /*bytes_transferred*/)>;
    using async_read_callback = delegate<void(op_result& /*function_result*/, byte* const /*recieve_buffer_address*/, net::size_t /*recieve_buffer_size*/, net::size_t /*bytes_transferred*/)>;
    virtual ~net_stream() = default;
    virtual void        connect(const char* ip_addr, port_t port) throw(...) = 0;
    virtual void        async_connect(const char* ip_addr, port_t port, async_connect_callback e);
    virtual void        async_connect(const char* ip_addr, port_t port, std::shared_ptr<async_connect_callback> e) = 0;
    virtual void        disconnect() = 0;
    virtual net::size_t write(const byte* data, net::size_t len) throw(...) = 0;
    virtual void        async_write(const byte* data, net::size_t len, async_write_callback e) throw(...);
    virtual void        async_write(const byte* data, net::size_t len, std::shared_ptr<async_write_callback> e) throw(...) = 0;
    virtual net::size_t read(byte* const buffer, net::size_t buf_capacity) throw(...) = 0;
    virtual void        async_read(byte* const buffer, net::size_t buf_capacity, async_read_callback e) throw(...);
    virtual void        async_read(byte* const buffer, net::size_t buf_capacity, std::shared_ptr<async_read_callback> e) throw(...) = 0;
};

class net_server abstract
{
public:
    virtual ~net_server() = default;
    virtual ::std::shared_ptr<net_stream> accept() throw(...) = 0;
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
    virtual ~ws_stream() = default;
    virtual void        connect(const char* ip_addr, port_t port) throw(...) = 0;
    virtual void        async_connect(const char* ip_addr, port_t port, std::shared_ptr<async_connect_callback> e) = 0;
    virtual void        disconnect() = 0;
    virtual net::size_t write(const byte* data, net::size_t len) throw(...) override;
    virtual io_result   write(opcode_id opcode, const byte* data, net::size_t len) throw(...) = 0;
    virtual void        async_write(const byte* data, net::size_t len, std::shared_ptr<async_write_callback> e) throw(...) override;
    virtual void        async_write(opcode_id opcode, const byte* data, net::size_t len, std::shared_ptr<async_write_callback> e) throw(...) = 0;
    virtual net::size_t read(byte* const buffer, net::size_t buf_capacity) throw(...) override;
    virtual io_result   read_payload_only(byte* const buffer, net::size_t buf_capacity) throw(...) = 0;
    virtual io_result   read_all(byte* const buffer, net::size_t buf_capacity) throw(...) = 0;
    virtual void        async_read(byte* const buffer, net::size_t buf_capacity, std::shared_ptr<async_read_callback> e) throw(...) override;
    virtual void        async_read_payload_only(byte* const buffer, net::size_t buf_capacity, std::shared_ptr<async_read_callback> e) throw(...) = 0;
    virtual void        async_read_all(byte* const buffer, net::size_t buf_capacity, std::shared_ptr<async_read_callback> e) throw(...) = 0;
};

::std::shared_ptr<net_server> create_server(unsigned short port);
::std::shared_ptr<ws_stream> create_stream();

} // namespace ws

} // namespace net
} // namespace voost
} // namespace vee

#endif // !_VEE_VOOST_NET_H_
