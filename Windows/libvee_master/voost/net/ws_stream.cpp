#include <array>
#include <vector>
#include <map>
#include <vee/voost/ws_stream.h>
#include <boost/tokenizer.hpp>

namespace vee {
namespace voost {
namespace net {
namespace websocket {

namespace /* unnamed */ {
inline ::boost::asio::ip::address string_to_ipaddr(const char* str)
{
    return ::boost::asio::ip::address::from_string(str);
}
} // unnamed namespace

client_header::client_header():
host("null"),
upgrade("null"),
connection("null"),
origin("null"),
sec_websocket_key("null"),
sec_websocket_protocol("null"),
sec_web_socket_version("null")
{

}

client_header::~client_header()
{

}

client_header::client_header(const client_header& other):
host(other.host),
upgrade(other.upgrade),
connection(other.connection),
origin(other.origin),
sec_websocket_key(other.sec_websocket_key),
sec_websocket_protocol(other.sec_websocket_protocol),
sec_web_socket_version(other.sec_web_socket_version)
{

}

client_header::client_header(client_header&& other):
host(static_cast<string&&>(other.host)),
upgrade(static_cast<string&&>(other.upgrade)),
connection(static_cast<string&&>(other.connection)),
origin(static_cast<string&&>(other.origin)),
sec_websocket_key(static_cast<string&&>(other.sec_websocket_key)),
sec_websocket_protocol(static_cast<string&&>(other.sec_websocket_protocol)),
sec_web_socket_version(static_cast<string&&>(other.sec_web_socket_version))
{

}

client_header& client_header::operator=(const client_header& rhs)
{
    host = rhs.host;
    upgrade = rhs.upgrade;
    connection = rhs.connection;
    origin = rhs.origin;
    sec_websocket_key = rhs.sec_websocket_key;
    sec_websocket_protocol = rhs.sec_websocket_protocol;
    sec_web_socket_version = rhs.sec_web_socket_version;
    return *this;
}

client_header& client_header::operator=(client_header&& rhs)
{
    host = static_cast<string&&>(rhs.host);
    upgrade = static_cast<string&&>(rhs.upgrade);
    connection = static_cast<string&&>(rhs.connection);
    origin = static_cast<string&&>(rhs.origin);
    sec_websocket_key = static_cast<string&&>(rhs.sec_websocket_key);
    sec_websocket_protocol = static_cast<string&&>(rhs.sec_websocket_protocol);
    sec_web_socket_version = static_cast<string&&>(rhs.sec_web_socket_version);
    return *this;
}

void client_header::print()
{
    printf("host: %s\n\
                upgrade: %s\n\
                      connection: %s\n\
                                 origin: %s\n\
                                            sec_websocket_key: %s\n\
                                                       sec_websocket_protocol: %s\n\
                                                                  sec_websocket_version: %s\n",
           host.data(),
           upgrade.data(),
           connection.data(),
           origin.data(),
           sec_websocket_key.data(),
           sec_websocket_protocol.data(),
           sec_web_socket_version.data()
           );
}

void client_header::parse(const char* data)
{
    
}

void client_header::parse(string& data)
{
    using char_separator = ::boost::char_separator<char>;
    using tokenizer = boost::tokenizer<char_separator>;

    // Parse raw data per lines (token: \n)
    ::std::vector<string> data_by_lines;
    {
        char_separator sep("\n");
        tokenizer tok(data, sep);
        for (auto& it : tok)
        {
            data_by_lines.push_back(it);
        }
    }
    //TODO: PARSING 
}

websocket_server::websocket_server(unsigned short port, io_service_t& io_service /* = io_service_sigleton::get().io_service() */):
_host_io_service(io_service),
_tcp_server(port, io_service)
{
    
}

websocket_server::websocket_server(websocket_server&& other):
_host_io_service(other._host_io_service),
_tcp_server(static_cast<websocket_server::tcp_server&&>(other._tcp_server))
{

}

websocket_server::~websocket_server()
{

}

void websocket_server::close()
{

}

::std::shared_ptr<net_stream> websocket_server::accept() throw(...)
{
    //TODO: RFC6455 HANDSHAKE
    auto tcp_stream = _tcp_server.accept();
    try
    {
        ::std::array<char, 4096> buffer;
        tcp_stream->read(buffer.data(), buffer.size());
    }
    catch (vee::exception& e)
    {
        printf("websocket_server> exception occured! %s\n", e.what());
    }
    ::std::shared_ptr<net_stream> stream(nullptr);
    return stream;
}

websocket_stream::websocket_stream():
_host_io_service(io_service_sigleton::get().io_service()),
_tcp_stream(_host_io_service)
{

}

websocket_stream::websocket_stream(io_service_t& io_service):
_host_io_service(io_service),
_tcp_stream(io_service)
{

}

websocket_stream::websocket_stream(tcp_stream&& stream):
_host_io_service(stream.get_io_service()),
_tcp_stream(static_cast<tcp_stream&&>(stream))
{
    
}

websocket_stream::~websocket_stream()
{

}

void websocket_stream::connect(const char* ip_addr, port_t port) throw(...)
{
    //nothing to do.
}

void websocket_stream::disconnect()
{
    _tcp_stream.disconnect();
}

net::size_t websocket_stream::write(void* buffer, net::size_t buf_capacity) throw(...)
{
    //TODO: ADD WEBSOCKET HEADER (RFC6455)
    // and call _tcp_stream.write()
    return 0;
}

net::size_t websocket_stream::read(void* buffer, net::size_t buf_capacity) throw(...)
{
    //TODO: ANALYZE WEBSOCKET HEADER
    // and call _tcp_stream.read()
    return 0;
}

::std::shared_ptr<server_interface> create_server(unsigned short port)
{
    ::std::shared_ptr<server_interface> server = ::std::make_shared<websocket_server>(port);
    return server;
}

::std::shared_ptr<net_stream> create_stream()
{
    //TODO: IO_SERVICE SELECTION
    ::std::shared_ptr<net_stream> stream = ::std::make_shared<websocket_stream>();
    return stream;
}

} // namespace ws
} // namespace net
} // namespace voost
} // namespace vee