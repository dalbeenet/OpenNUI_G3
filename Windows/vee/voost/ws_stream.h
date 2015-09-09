#ifndef _VEE_VOOST_WS_STREAM_H_
#define _VEE_VOOST_WS_STREAM_H_
#include <vee/voost/tcp_stream.h>
#include <vee/string.h>

namespace vee {
namespace voost {
namespace net {
namespace websocket {

class websocket_server;
class websocket_stream;

struct client_header
{
    client_header();
    ~client_header();
    client_header(const client_header&);
    client_header(client_header&&);
    client_header& operator=(const client_header&);
    client_header& operator=(client_header&&);
    void print();
    void parse(const char* data);
    void parse(string& data);
    string host;
    string upgrade;
    string connection;
    string origin;
    string sec_websocket_key;
    string sec_websocket_protocol;
    string sec_web_socket_version;
};

class websocket_server: public server_interface
{
    DISALLOW_COPY_AND_ASSIGN(websocket_server);
public:
    using tcp_server = tcp::tcp_server;
    using tcp_stream = tcp::tcp_stream;
    using socket_t = ::boost::asio::ip::tcp::socket;
    using endpoint = ::boost::asio::ip::tcp::endpoint;
    using io_service_t = ::boost::asio::io_service;
    websocket_server(unsigned short port, io_service_t& io_service = io_service_sigleton::get().io_service());
    websocket_server(websocket_server&& other);
    virtual ~websocket_server();
    virtual void close() override;
    virtual ::std::shared_ptr<net_stream> accept() throw(...) override;
    inline io_service_t& get_io_service() const { return _host_io_service; }
protected:
    ::boost::asio::io_service& _host_io_service;
    tcp_server _tcp_server;
};

class websocket_stream: public net_stream
{
    DISALLOW_COPY_AND_ASSIGN(websocket_stream);
public:
    using tcp_stream = tcp::tcp_stream;
    using socket_t = ::boost::asio::ip::tcp::socket;
    using endpoint = ::boost::asio::ip::tcp::endpoint;
    using io_service_t = ::boost::asio::io_service;
    websocket_stream();
    explicit websocket_stream(tcp_stream&& stream);
    explicit websocket_stream(io_service_t& io_service);
    virtual ~websocket_stream();
    virtual void connect(const char* ip_addr, port_t port) throw(...) override;
    virtual void disconnect() override;
    virtual net::size_t write(void* buffer, net::size_t buf_capacity) throw(...) override;
    virtual net::size_t read(void* buffer, net::size_t buf_capacity) throw(...) override;
    inline io_service_t& get_io_service() const { return _host_io_service; }
protected:
    io_service_t& _host_io_service;
    tcp_stream _tcp_stream;
};

} // namespace ws
} // namespace net
} // namespace voost
} // namespace vee

#endif // _VEE_VOOST_WS_STREAM_H_