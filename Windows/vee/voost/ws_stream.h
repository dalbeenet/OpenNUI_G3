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

struct RFC4122_GUID
{
    // Returns Globally Unique Identifier, GUID
    inline static const char* get()
    {
        return "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    }
};

struct RFC6455_client_handshake_header
{
    RFC6455_client_handshake_header();
    ~RFC6455_client_handshake_header();
    RFC6455_client_handshake_header(const RFC6455_client_handshake_header&);
    RFC6455_client_handshake_header(RFC6455_client_handshake_header&&);
    RFC6455_client_handshake_header& operator=(const RFC6455_client_handshake_header&);
    RFC6455_client_handshake_header& operator=(RFC6455_client_handshake_header&&);
    void print() const;
    void parse(const char* data);
    void parse(string& data);
    bool is_valid() const;
    void clear();
    string request_uri;
    string host;
    string upgrade;
    string connection;
    string origin;
    string sec_websocket_key;
    string sec_websocket_protocol;
    string sec_web_socket_version;
};

struct RFC6455_server_response
{
    RFC6455_server_response();
    ~RFC6455_server_response();
    RFC6455_server_response(const RFC6455_server_response&);
    RFC6455_server_response(RFC6455_server_response&&);
    RFC6455_server_response(string& secret_key);
    RFC6455_server_response& operator=(const RFC6455_server_response&);
    RFC6455_server_response& operator=(RFC6455_server_response&&);
    void print() const;
    void clear();
    string pack() const;
    string http_status;
    string upgrade;
    string connection;
    string sec_websocket_accept;
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
    bool _handshake(net_stream& raw_socket);
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
    virtual ~websocket_stream();
    websocket_stream& operator=(websocket_stream&& rhs);
    virtual void connect(const char* ip_addr, port_t port) throw(...) override;
    virtual void disconnect() override;
    virtual net::size_t write(void* buffer, net::size_t buf_capacity) throw(...) override;
    virtual net::size_t read(void* buffer, net::size_t buf_capacity) throw(...) override;
    inline io_service_t& get_io_service() const { return *_host_io_service_ptr; }
    void conversion(tcp_stream&& stream);
protected:
    io_service_t* _host_io_service_ptr;
    tcp_stream _tcp_stream;
};

} // namespace ws
} // namespace net
} // namespace voost
} // namespace vee

#endif // _VEE_VOOST_WS_STREAM_H_