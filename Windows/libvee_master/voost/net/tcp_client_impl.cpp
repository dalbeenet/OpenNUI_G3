#include <vee/voost/net.h>

_VEE_BEGIN
_VOOST_BEGIN

const char* local_ip = "127.0.0.1";

tcp_client::tcp_client(::boost::asio::io_service& io_service):
_io_service(io_service),
_socket(io_service)
{
    // empty
}

tcp_client::~tcp_client()
{
    if (is_open())
    {
        close();
    }
}

bool tcp_client::connect(::boost::asio::ip::tcp::endpoint& endpoint)
{
    ::boost::system::error_code error;
    _socket.connect(endpoint, error);
    if (error)
    {
        return false;
    }
    return true;
}

::std::size_t tcp_client::write(char* buffer, ::std::size_t len)
{
    ::boost::system::error_code ignored_error;
    ::std::size_t write_len = _socket.write_some(::boost::asio::buffer(buffer, len), ignored_error);
    if (ignored_error)
    {
        printf("WTF!\n");
    }
    return write_len;
}

::std::size_t tcp_client::read(char* buffer, ::std::size_t len)
{
    ::boost::system::error_code error;
    ::std::size_t read_len = _socket.read_some(::boost::asio::buffer(buffer, len), error);
    if (error)
    {
        if (error == ::boost::asio::error::eof)
        {
            return 0;
        }
        return 0;
    }
    return read_len;
}

bool tcp_client::is_open() const
{
    return _socket.is_open();
}

void tcp_client::close()
{
    _socket.close();
}

_VOOST_END
_VEE_END