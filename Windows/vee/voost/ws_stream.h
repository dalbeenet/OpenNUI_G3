#ifndef _VEE_VOOST_WS_STREAM_H_
#define _VEE_VOOST_WS_STREAM_H_
#include <vee/voost/tcp_stream.h>

namespace vee {
namespace voost {
namespace net {
namespace ws {

//class ws_stream;
//class ws_server;

//class ws_server: public tcp::tcp_server
//{
//    DISALLOW_COPY_AND_ASSIGN(ws_server);
//public:
//    using parent_t = tcp::tcp_server;
//    using socket_t = parent_t::socket_t;
//    using endpoint = parent_t::endpoint;
//    ws_server(unsigned short port, ::boost::asio::io_service& io_service = io_service_sigleton::get().io_service());
//    ws_server(ws_server&& other);
//    virtual ~ws_server();
//    //virtual void close() override;
//    virtual ::std::shared_ptr<net_stream> accept() throw(...) override;
//};
//
//class ws_stream: public tcp::tcp_stream
//{
//    DISALLOW_COPY_AND_ASSIGN(ws_stream);
//public:
//    using parent_t = tcp::tcp_stream;
//    using socket_t = parent_t::socket_t;
//    using endpoint = parent_t::endpoint;
//    ws_stream();
//    explicit ws_stream(::boost::asio::io_service& io_service);
//    ws_stream(ws_stream&& other);
//    virtual ~ws_stream();
//    virtual void connect(const char* ip_addr, port_t port) throw(...) override;
//    virtual void disconnect() override;
//    virtual net::size_t write(void* buffer, net::size_t len) throw(...) override;
//    virtual net::size_t read(void* buffer, net::size_t len) throw(...) override;
//};

} // namespace ws
} // namespace net
} // namespace voost
} // namespace vee

#endif // _VEE_VOOST_WS_STREAM_H_