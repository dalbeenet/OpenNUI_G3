#ifndef _VEE_VOOST_NET_H_
#define _VEE_VOOST_NET_H_

#include <vee/macro.h>
#include <boost/asio.hpp>
#include <exception>

_VEE_BEGIN
_VOOST_BEGIN

extern const char* local_ip;

typedef ::boost::asio::ip::tcp::endpoint tcp_endpoint;
typedef ::boost::asio::ip::udp::endpoint udp_endpoint;

inline ::boost::asio::ip::address string_to_ipaddr(const char* str)
{
    return ::boost::asio::ip::address::from_string(str);
}

class tcp_client
{
public:
    tcp_client(::boost::asio::io_service& io_service);
    ~tcp_client();
    //TODO: copy ctor and move ctor
    //TODO: operator = 
    bool connect(::boost::asio::ip::tcp::endpoint& endpoint);
    ::std::size_t write(char* buffer, ::std::size_t len);
    ::std::size_t read(char* buffer, ::std::size_t len);
    bool is_open() const;
    void close();
protected:
    ::boost::asio::io_service& _io_service;
    ::boost::asio::ip::tcp::socket _socket;
};

_VOOST_END
_VEE_END

#endif // !_VEE_VOOST_NET_H_
