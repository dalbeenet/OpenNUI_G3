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
    virtual net::size_t write(void* data, size_t len) throw(...) = 0;
    virtual net::size_t read(void* buffer, size_t buf_capacity) throw(...) = 0;
};

enum class error_code: int
{
    connection_failure = 1,
    send_failure,
    recv_failure,
    invalid_data,
    connection_closed,
    user,
};

namespace tcp {

class server_interface abstract
{
public:
    virtual ~server_interface() = default;
    virtual ::std::shared_ptr<net_stream> accept() throw(...) = 0;
    virtual void close() = 0;
};

::std::shared_ptr<server_interface> create_server(unsigned short port);
::std::shared_ptr<net_stream> create_stream();

} // namespace tcp

namespace udp {

} // namespace udp

namespace websocket {

class server_interface abstract
{
public:
    virtual ~server_interface() = default;
    virtual ::std::shared_ptr<net_stream> accept() throw(...) = 0;
    virtual void close() = 0;
};

::std::shared_ptr<server_interface> create_server(unsigned short port);
::std::shared_ptr<net_stream> create_stream();

} // namespace ws

} // namespace net
} // namespace voost
} // namespace vee

#endif // !_VEE_VOOST_NET_H_
