#include <vee/voost/ws_stream.h>

namespace vee {
namespace voost {
namespace net {
namespace ws {

namespace /* unnamed */ {
inline ::boost::asio::ip::address string_to_ipaddr(const char* str)
{
    return ::boost::asio::ip::address::from_string(str);
}
} // unnamed namespace

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
}

net::size_t websocket_stream::read(void* buffer, net::size_t buf_capacity) throw(...)
{
    //TODO: ANALYZE WEBSOCKET HEADER
    // and call _tcp_stream.read()
}

} // namespace ws
} // namespace net
} // namespace voost
} // namespace vee