#include <vee/voost/tcp_stream.h>

namespace vee {
namespace voost {
namespace net {
namespace tcp {

namespace /* unnamed */ {
inline ::boost::asio::ip::address string_to_ipaddr(const char* str)
{
    return ::boost::asio::ip::address::from_string(str);
}
} // unnamed namespace

tcp_server::tcp_server(unsigned short port, ::boost::asio::io_service& io_service):
_host_io_service(io_service),
_socket(_host_io_service),
_endpoint(boost::asio::ip::tcp::v4(), port),
_acceptor(_host_io_service, _endpoint)
{

}

tcp_server::tcp_server(tcp_server&& other):
_host_io_service(other._host_io_service),
_socket(static_cast<socket_t&&>(other._socket)),
_endpoint(static_cast<endpoint&&>(other._endpoint)),
_acceptor(static_cast<::boost::asio::ip::tcp::acceptor&&>(other._acceptor))
{

}

tcp_server::~tcp_server()
{
    
}

void tcp_server::close()
{
    _socket.close();
}

::std::shared_ptr<net_stream> tcp_server::accept()
{
    ::boost::asio::ip::tcp::socket client(_host_io_service);
    _acceptor.accept(client);
    ::std::shared_ptr<net_stream> ret = ::std::make_shared<tcp_stream>(std::move(client));
    return ret;
}

tcp_stream::tcp_stream():
_host_io_service(io_service_sigleton::get().io_service()),
_socket(_host_io_service)
{

}

tcp_stream::tcp_stream(::boost::asio::ip::tcp::socket&& socket):
_host_io_service(socket.get_io_service()),
_socket(std::move(socket))
{
    
}

tcp_stream::tcp_stream(::boost::asio::io_service& io_service):
_host_io_service(io_service),
_socket(_host_io_service)
{

}

tcp_stream::tcp_stream(tcp_stream&& other):
_host_io_service(other._host_io_service),
_socket(static_cast<socket_t&&>(other._socket))
{
    
}

tcp_stream::~tcp_stream()
{
    //puts(__FUNCSIG__);
}

tcp_stream& tcp_stream::operator=(tcp_stream&& rhs)
{
    //_host_io_service = rhs._host_io_service;
    //_socket = static_cast<socket_t&&>(rhs._socket);
    ::std::swap(_socket, rhs._socket);
    return *this;
}

void tcp_stream::connect(const char* ip_addr, port_t port) throw(...)
{
    ::boost::system::error_code error;
    endpoint ep(string_to_ipaddr(ip_addr), port);
    _socket.connect(ep, error);
    if (error)
    {
        throw ::vee::exception("Connection failure!", (int)error_code::connection_failure);
    }
}

net::size_t tcp_stream::write(void* data, net::size_t len) throw(...)
{
    ::boost::system::error_code error;
    net::size_t write_len = (net::size_t)_socket.write_some(::boost::asio::buffer(data, (uint32_t)len), error);
    if (error)
    {
        throw ::vee::exception("Send failure!", (int)error_code::send_failure);
    }
    return write_len;
}

net::size_t tcp_stream::read(void* buffer, net::size_t buf_capacity) throw(...)
{
    ::boost::system::error_code error;
    memset(buffer, 0, (uint32_t)buf_capacity);
    net::size_t read_len = (net::size_t)_socket.read_some(::boost::asio::buffer(buffer, (uint32_t)buf_capacity), error);
    if (error)
    {
        throw ::vee::exception("Recv failure!", (int)error_code::recv_failure);
    }
    return read_len;
}

void tcp_stream::disconnect()
{
    _socket.close();
}

::std::shared_ptr<net_server> create_server(unsigned short port)
{
    ::std::shared_ptr<net_server> server = ::std::make_shared<tcp_server>(port);
    return server;
}

::std::shared_ptr<net_stream> create_stream()
{
    //TODO: IO_SERVICE SELECTION
    ::std::shared_ptr<net_stream> stream = ::std::make_shared<tcp_stream>();
    return stream;
}

} // namespace tcp
} // namespace net
} // namespace voost
} // namespace vee