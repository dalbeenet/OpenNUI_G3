#ifndef _VEE_VOOST_NET_H_
#define _VEE_VOOST_NET_H_

#include <vee/macro.h>
#include <vee/exception.h>
#include <memory>

namespace vee {
namespace voost {
namespace net {

using port_t = unsigned short;
using size_t = unsigned int;
using byte = unsigned char;

class net_stream abstract
{
public:
    virtual ~net_stream() = default;
    virtual void connect(const char* ip_addr, port_t port) throw(...) = 0;
    virtual void disconnect() = 0;
    virtual size_t write(void* buffer, size_t len) throw(...) = 0;
    virtual size_t read(void* buffer, size_t len) throw(...) = 0;
};

namespace tcp {

enum class error_code: int
{
    connection_failure = 1,
    send_failure,
    recv_failure,
    invalid_data,
    user,
};

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

enum class error_code: int
{
    bad_request = 400,
};

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
