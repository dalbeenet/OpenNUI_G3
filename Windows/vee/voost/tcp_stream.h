#ifndef _VEE_VOOST_TCP_STREAM_H_
#define _VEE_VOOST_TCP_STREAM_H_
#include <vee/voost/net.h>
#include <vee/voost/asio.h>

namespace vee {
namespace voost {
namespace net {
namespace tcp {

class tcp_server;
class tcp_stream;

class tcp_server: public net_server
{
    DISALLOW_COPY_AND_ASSIGN(tcp_server);
public:
    using socket_t = ::boost::asio::ip::tcp::socket;
    using endpoint = ::boost::asio::ip::tcp::endpoint;
    using io_service_t = ::boost::asio::io_service;
    tcp_server(unsigned short port, io_service_t& io_service = io_service_sigleton::get().io_service());
    tcp_server(tcp_server&& other);
    virtual ~tcp_server();
    virtual void close() override;
    virtual ::std::shared_ptr<net_stream> accept() throw(...) override;
    virtual void async_accept(std::function<_vee_net_async_accept_callback_sig> e) override;
    inline io_service_t& get_io_service() const { return _host_io_service; }
protected:
    io_service_t& _host_io_service;
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
    using io_service_t = ::boost::asio::io_service;
    tcp_stream();
    virtual ~tcp_stream();
    explicit tcp_stream(socket_t&& socket);
    explicit tcp_stream(io_service_t& io_service);
    tcp_stream(tcp_stream&& other);
    tcp_stream& operator=(tcp_stream&& rhs);
    virtual void        connect(const char* ip_addr, port_t port) throw(...) override;
    virtual void        async_connect(const char* ip_addr, port_t port, async_connect_callback e) override;
    virtual void        disconnect() override;
    virtual uint32_t    write(const byte* data, const uint32_t len) throw(...) override;
    virtual void        async_write(const byte* data, const uint32_t len, async_write_callback e) throw(...) override;
    virtual uint32_t    read(byte* const buffer, const uint32_t buf_capacity) throw(...) override;
    virtual void        async_read(byte* const buffer, const uint32_t buf_capacity, async_read_callback e) throw(...) override;
    inline io_service_t& get_io_service() const
    {
        return _host_io_service;
    }
protected:
    io_service_t& _host_io_service;
    socket_t _socket;
};

} // namespace tcp
} // namespace net
} // namespace voost
} // namespace vee

#endif // _VEE_VOOST_TCP_STREAM_H_