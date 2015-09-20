#ifndef _VEE_VOOST_NET_H_
#define _VEE_VOOST_NET_H_

#include <vee/macro.h>
#include <vee/exception.h>
#include <memory>

namespace vee {
namespace voost {
namespace net {

using port_t = unsigned short;
using size_t = unsigned long long;
using byte = unsigned char;

class net_stream abstract
{
public:
    virtual ~net_stream() = default;
    virtual void connect(const char* ip_addr, port_t port) throw(...) = 0;
    virtual void disconnect() = 0;
    virtual net::size_t write(void* data, net::size_t len) throw(...) = 0;
    virtual net::size_t read(void* buffer, net::size_t buf_capacity) throw(...) = 0;
};

class net_server abstract
{
public:
    virtual ~net_server() = default;
    virtual ::std::shared_ptr<net_stream> accept() throw(...) = 0;
    virtual void close() = 0;
};

enum class error_code: int
{
    connection_failure = 1,
    send_failure,
    recv_failure,
    invalid_data,
    disconnected_by_host,
    client_handshake_failure,
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
    virtual void        disconnect() = 0;
    virtual net::size_t write(void* data, net::size_t len) throw(...) override;
    virtual io_result   write(opcode_id opcode, void* data, net::size_t len) throw(...) = 0;
    virtual net::size_t read(void* buffer, net::size_t buf_capacity) throw(...) override;
    virtual io_result   read_payload_only(void* buffer, net::size_t buf_capacity) throw(...) = 0;
    virtual io_result   read_all(void* buffer, net::size_t buf_capacity) throw(...) = 0;
};

::std::shared_ptr<net_server> create_server(unsigned short port);
::std::shared_ptr<ws_stream> create_stream();

} // namespace ws

} // namespace net
} // namespace voost
} // namespace vee

#endif // !_VEE_VOOST_NET_H_
