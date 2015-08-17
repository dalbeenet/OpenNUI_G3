#ifndef _VEE_VOOST_STREAM_H_
#define _VEE_VOOST_STREAM_H_
#include <vee/voost/net.h>
#include <boost/asio.hpp>

namespace vee {
namespace voost {
namespace net {

extern ::boost::asio::io_service net_io_service;

namespace tcp {

class tcp_server;
class tcp_stream;

class tcp_server: public tcp_server_controller
{
    DISALLOW_COPY_AND_ASSIGN(tcp_server);
public:
    using socket_t = ::boost::asio::ip::tcp::socket;
    using endpoint = ::boost::asio::ip::tcp::endpoint;
    tcp_server(unsigned short port);
    tcp_server(unsigned short port, ::boost::asio::io_service& io_service);
    tcp_server(tcp_server&& other);
    virtual ~tcp_server();
    virtual void close() override;
    virtual ::std::shared_ptr<net_stream> accept() throw(...) override;
protected:
    ::boost::asio::io_service& _host_io_service;
    ::boost::asio::ip::tcp::endpoint _endpoint;
    ::boost::asio::ip::tcp::acceptor _acceptor;
    socket_t _socket;
};

class tcp_stream: public net_stream
{
    DISALLOW_COPY_AND_ASSIGN(tcp_stream);
public:
    using socket_t = ::boost::asio::ip::tcp::socket;
    using endpoint = ::boost::asio::ip::tcp::endpoint;
    tcp_stream();
    explicit tcp_stream(socket_t&& socket);
    explicit tcp_stream(::boost::asio::io_service& io_service);
    tcp_stream(tcp_stream&& other);
    virtual ~tcp_stream();
    virtual void connect(const char* ip_addr, port_t port) throw(...) override;
    virtual void disconnect() override;
    virtual net::size_t write(void* buffer, net::size_t len) throw(...) override;
    virtual net::size_t read(void* buffer, net::size_t len) throw(...) override;
public:
    ::boost::asio::io_service& _host_io_service;
    socket_t _socket;
};

} // namespace tcp
} // namespace net
} // namespace voost
} // namespace vee

#endif // _VEE_VOOST_STREAM_H_