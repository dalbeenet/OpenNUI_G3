#include <iostream>
#include <array>
#include <vector>
#include <map>
#include <vee/voost/ws_stream.h>
#include <vee/voost/string.h>
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
request_uri("null"),
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
request_uri(other.request_uri),
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
request_uri(static_cast<string&&>(other.request_uri)),
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
    request_uri = rhs.request_uri;
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
    request_uri = static_cast<string&&>(rhs.request_uri);
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
    printf("Request-Uri: %s\nHost: %s\nUpgrade: %s\nConnection: %s\nOrigin: %s\nSec_websocket_key: %s\nSec_websocket_protocol: %s\nSec_websocket_version: %s\n",
           request_uri.data(),
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
    return parse(make_string(data));
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

    auto get_value = [](const string& dst)-> std::pair<bool, string>
    {
        auto pos = dst.find(':');
        if (pos == string::npos)
        {
            return std::make_pair(false, "");
        }
        else
        {
            string buffer(dst.substr(pos + 1));
            string result = trim(buffer);
            return std::make_pair(true, std::move(result));
        }
    };

    /*printf("DATA RECIEVED --------------\n");
    for (auto& it : data_by_lines)
    {
    printf("%s\n", it.data());
    }*/

    for (auto& it : data_by_lines)
    {
        auto set_value = [](::std::pair<bool, string>& result, string& dst) -> void
        {
            if (result.first == true)
                dst = std::move(result.second);
        };

        if (strstr(it.data(), "GET"))
        {
            request_uri = it;
        }
        else if (strstr(it.data(), "Host:"))
        {
            set_value(get_value(it), host);
        }
        else if (strstr(it.data(), "Upgrade:"))
        {
            set_value(get_value(it), upgrade);
        }
        else if (strstr(it.data(), "Connection:"))
        {
            set_value(get_value(it), connection);
        }
        else if (strstr(it.data(), "Sec-WebSocket-Key:"))
        {
            set_value(get_value(it), sec_websocket_key);
        }
        else if (strstr(it.data(), "Origin:"))
        {
            set_value(get_value(it), origin);
        }
        else if (strstr(it.data(), "Sec-WebSocket-Protocol:"))
        {
            set_value(get_value(it), sec_websocket_protocol);
        }
        else if (strstr(it.data(), "Sec-WebSocket-Version:"))
        {
            set_value(get_value(it), sec_web_socket_version);
        }
        else
        {
            continue;
        }
    }
}

bool client_header::is_valid() const
{
    //! 다음이 생략됨
    //! HOST 헤더 유효성 검사
    //! Sec-Websocket-Key 헤더 유효성 검사
    //! Origin 헤더 유효성 검사
    if (
       (strtool::find_case_insensitive(request_uri.data(), "GET") == strtool::not_found) ||
       (strtool::find_case_insensitive(request_uri.data(), "HTTP/1.1") == strtool::not_found) ||
       (strtool::find_case_insensitive(upgrade.data(), "Websocket") == strtool::not_found) ||
       (strtool::find_case_insensitive(connection.data(), "Upgrade") == strtool::not_found) ||
       (strtool::find_case_insensitive(sec_web_socket_version.data(), "13") == strtool::not_found)
       )
    {
        return false;
    }
    return true;
}

void client_header::clear()
{
    request_uri.clear();
    host.clear();
    upgrade.clear();
    connection.clear();
    origin.clear();
    sec_websocket_key.clear();
    sec_web_socket_version.clear();
    sec_websocket_protocol.clear();
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
    auto raw_stream = _tcp_server.accept();
    bool handshake_result = _handshake(*raw_stream);
    ::std::shared_ptr<net_stream> stream(nullptr);
    return stream;
}

bool websocket_server::_handshake(net_stream& raw_socket)
{
    try
    {
        ::std::array<char, 4096> buffer;
        raw_socket.read(buffer.data(), buffer.size());
        
        // Parsing HTTP header
        client_header header;
        header.parse(make_string(buffer.data()));
        
        // Validating the client header
        if (header.is_valid() == false)
        {
            printf("Invalid header!\n");
            header.print();
            return false;
        }

        printf("Header information -------\n");
        header.print();
        
        string magic_string(header.sec_websocket_key);
        magic_string.append(RFC4122_GUID::get());
        // Hashing
        auto hash_code = sha1_hashing(magic_string);
        print_hash_code(hash_code.data());
    }
    catch (vee::exception& e)
    {
        printf("websocket_server> exception occured at _handshake() %s\n", e.what());
        return false;
    }
    return true;
}

websocket_stream::websocket_stream(tcp_stream&& stream, io_service_t& io_service /* = io_service_sigleton::get().io_service() */):
_host_io_service(io_service),
_tcp_stream(static_cast<tcp_stream&&>(stream))
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