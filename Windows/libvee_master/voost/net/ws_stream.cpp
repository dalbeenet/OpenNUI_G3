#include <iostream>
#include <array>
#include <vector>
#include <map>
#include <vee/voost/ws_stream.h>
#include <vee/voost/string.h>
#include <boost/tokenizer.hpp>
#include <boost/endian/conversion.hpp>

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

handshake_client_request::handshake_client_request():
request_uri(""),
host(""),
upgrade(""),
connection(""),
origin(""),
sec_websocket_key(""),
sec_websocket_protocol(""),
sec_web_socket_version("")
{

}

handshake_client_request::~handshake_client_request()
{

}

handshake_client_request::handshake_client_request(const handshake_client_request& other):
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

handshake_client_request::handshake_client_request(handshake_client_request&& other):
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

handshake_client_request& handshake_client_request::operator=(const handshake_client_request& rhs)
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

handshake_client_request& handshake_client_request::operator=(handshake_client_request&& rhs)
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

void handshake_client_request::print() const
{
    printf("%s\nHost: %s\nUpgrade: %s\nConnection: %s\nOrigin: %s\nSec-Websocket-Key: %s\nSec-Websocket-Protocol: %s\nSec-Websocket-Version: %s\n",
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

void handshake_client_request::parse(const char* data)
{
    return parse(make_string(data));
}

void handshake_client_request::parse(string& data)
{
    using char_separator = ::boost::char_separator<char>;
    using tokenizer = boost::tokenizer<char_separator>;

    // Parse raw data per lines (token: \n)
    ::std::vector<string> data_by_lines;
    {
        char_separator sep("\r\n");
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
            {
                dst = std::move(result.second);
            }
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

bool handshake_client_request::is_valid() const
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

void handshake_client_request::clear()
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

string handshake_client_request::binary_pack() const
{
    string pack;
    pack.append(request_uri);
    pack.append("\r\nHost: ");
    pack.append(host);
    pack.append("\r\nUpgrade: ");
    pack.append(upgrade);
    pack.append("\r\nConnection: ");
    pack.append(connection);
    pack.append("\r\nSec-WebSocket-Key: ");
    pack.append(sec_websocket_key);
    pack.append("\r\nOrigin: ");
    pack.append(origin);
    pack.append("\r\nSec-WebSocket-Protocol: ");
    pack.append(sec_websocket_protocol);
    pack.append("\r\nSec-WebSocket-Version: ");
    pack.append(sec_web_socket_version);
    pack.append("\r\n\r\n");
    return pack;
}

handshake_server_response::handshake_server_response():
http_status("HTTP/1.1 101 Switching Protocols"),
upgrade("websocket"),
connection("Upgrade"),
sec_websocket_accept("")
{

}

handshake_server_response::handshake_server_response(const handshake_server_response& other):
http_status(other.http_status),
upgrade(other.upgrade),
connection(other.connection),
sec_websocket_accept(other.sec_websocket_accept)
{

}

handshake_server_response::handshake_server_response(handshake_server_response&& other):
http_status(static_cast<string&&>(other.http_status)),
upgrade(static_cast<string&&>(other.upgrade)),
connection(static_cast<string&&>(other.connection)),
sec_websocket_accept(static_cast<string&&>(other.sec_websocket_accept))
{

}

handshake_server_response::handshake_server_response(string& secret_key):
http_status("HTTP/1.1 101 Switching Protocols"),
upgrade("websocket"),
connection("Upgrade"),
sec_websocket_accept(secret_key)
{

}

handshake_server_response::~handshake_server_response()
{

}

handshake_server_response& handshake_server_response::operator=(const handshake_server_response& rhs)
{
    http_status = rhs.http_status;
    upgrade = rhs.upgrade;
    connection = rhs.connection;
    sec_websocket_accept = rhs.sec_websocket_accept;
    return *this;
}

handshake_server_response& handshake_server_response::operator=(handshake_server_response&& rhs)
{
    http_status = std::move(rhs.http_status);
    upgrade = std::move(rhs.upgrade);
    connection = std::move(rhs.connection);
    sec_websocket_accept = std::move(rhs.sec_websocket_accept);
    return *this;
}

void handshake_server_response::parse(const char* data)
{
    return parse(make_string(data));
}

void handshake_server_response::parse(string& data)
{
    using char_separator = ::boost::char_separator<char>;
    using tokenizer = boost::tokenizer<char_separator>;

    // Parse raw data per lines (token: \n)
    ::std::vector<string> data_by_lines;
    {
        char_separator sep("\r\n");
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

    for (auto& it : data_by_lines)
    {
        auto set_value = [](::std::pair<bool, string>& result, string& dst) -> void
        {
            if (result.first == true)
            {
                dst = std::move(result.second);
            }
        };

        if (strstr(it.data(), "HTTP/1.1 101 Switching Protocols"))
        {
            http_status = it;
        }
        else if (strstr(it.data(), "Upgrade:"))
        {
            set_value(get_value(it), upgrade);
        }
        else if (strstr(it.data(), "Connection:"))
        {
            set_value(get_value(it), connection);
        }
        else if (strstr(it.data(), "Sec-WebSocket-Accept:"))
        {
            set_value(get_value(it), sec_websocket_accept);
        }
        else
        {
            continue;
        }
    }
}

void handshake_server_response::print() const
{
    printf("%s\nUpgrade: %s\nConnection: %s\nSec-Websocket-Accept: %s\n", http_status.data(), upgrade.data(), connection.data(), sec_websocket_accept.data());
}

void handshake_server_response::clear()
{
    http_status = "HTTP/1.1 101 Switching Protocols";
    upgrade = "websocket";
    connection = "Upgrade";
    sec_websocket_accept.clear();
}

string handshake_server_response::binary_pack() const
{
    string ret;
    //ret += "HTTP/1.1 101 Switching Protocols\r\nConnection: Upgrade\r\nUpgrade: websocket\r\nSec-WebSocket-Origin: http://jjoriping.ufree.kr\r\nSec-WebSocket-Location: ws://localhost:8001/\r\nSec-WebSocket-Accept: ";
    ret.append(http_status);
    ret.append("\r\nUpgrade: ");
    ret.append(upgrade);
    ret.append("\r\nConnection: ");
    ret.append(connection);
    ret.append("\r\nSec-WebSocket-Accept: ");
    ret.append(sec_websocket_accept);
    //ret.append("\r\nSec-WebSocket-Location: ws://localhost:1992/");
    ret.append("\r\n\r\n");
    return ret;
}

void data_frame_header::analyze(const unsigned char* raw_data, const uint32_t length)
{
    if (!raw_data) return;

    unsigned int shift = 0;
    // analyze a first byte
    memmove(&fin_opcode, raw_data + shift++, 1);
    fin = ((fin_opcode & 0x80) >> 7) ? true : false;
    switch ((fin_opcode & 0x0f))
    {
    case 0x0:
        opcode = opcode_id::continuation_frame;
        break;
    case 0x1:
        opcode = opcode_id::text_frame;
        break;
    case 0x2:
        opcode = opcode_id::binary_frame;
        break;
    case 0x8:
        opcode = opcode_id::connnection_close;
        break;
    case 0x9:
        opcode = opcode_id::ping;
        break;
    case 0xA:
        opcode = opcode_id::pong;
        break;
    default:
        opcode = opcode_id::reserved_for_further;
        break;
    }
    //TODO: RSV1~3 (Extension flag) processing 

    // analyze a second byte
    memmove(&mask_payload_len, raw_data + shift++, 1);
    use_mask = ((mask_payload_len & 0x80) >> 7) ? true : false;
    payload_len = (mask_payload_len & 0x7f);
    if (payload_len == 0x7e)
    {
        memmove(&extended_payload_len16, raw_data + shift, 2);
        payload_len = boost::endian::big_to_native(extended_payload_len16);
        shift += 2;
        //x use_extended16_payload = true;
    }
    else if (payload_len == 0x7f)
    {
        memmove(&extended_payload_len64, raw_data + shift, 8);
        payload_len = boost::endian::big_to_native(extended_payload_len64);
        shift += 8;
        //x use_extended64_payload = true;
    }
    else
    {
        //x use_extended16_payload = false;
        //x use_extended64_payload = false;
    }

    // Getting a mask key
    if (use_mask == true)
    {
        //x ::std::array<unsigned char, 4> mask_key_buffer;
        //memmove(mask_key_buffer.data(), raw_data + shift, 4);
        //x std::reverse_copy(mask_key_buffer.begin(), mask_key_buffer.end(), masking_key.begin());
        memmove(masking_key.data(), raw_data + shift, 4);
        shift += 4;
    }

    payload_pos = shift;

    //x Getting a payload data
    //x payload.clear();
    //x payload.resize(length - shift);
    //x memmove(&payload[0], raw_data + shift, length - shift);

    return;
}

uint32_t data_frame_header::binary_pack(opcode_id opcode, unsigned char* out_buffer) const
{
    uint32_t shift = 0;

    int8_t fin_opcode_block = 0; 
    switch (opcode)
    {
    case vee::voost::net::websocket::opcode_id::continuation_frame:
        fin_opcode_block = 0x0;
        break;
    case vee::voost::net::websocket::opcode_id::text_frame:
        fin_opcode_block = 0x1;
        break;
    case vee::voost::net::websocket::opcode_id::binary_frame:
        fin_opcode_block = 0x2;
        break;
    case vee::voost::net::websocket::opcode_id::connnection_close:
        fin_opcode_block = 0x8;
        break;
    case vee::voost::net::websocket::opcode_id::ping:
        fin_opcode_block = 0x9;
        break;
    case vee::voost::net::websocket::opcode_id::pong:
        fin_opcode_block = 0xA;
        break;
    case vee::voost::net::websocket::opcode_id::reserved_for_further:
        fin_opcode_block = 0x2; // Websocket stream does not support this operation. set to default value(binary frame).
        break;
    default:
        fin_opcode_block = 0x2; // defualt value is binary frame
        break;
    }

    (fin) ? (fin_opcode_block |= 0x80) : (fin_opcode_block |= 0x00);
    memmove(out_buffer + shift++, &fin_opcode_block, 1);

    int8_t mask_payload_len_block = 0;
    if (payload_len >= 0x7e)
    {
        if (payload_len <= INT16_MAX)
            mask_payload_len_block = 0x7e;
        else
            mask_payload_len_block = 0x7f;
    }
    else
        mask_payload_len_block = (int8_t)payload_len;
    (use_mask) ? (mask_payload_len_block |= 0x80) : (mask_payload_len_block |= 0x00);
    memmove(out_buffer + shift++, &mask_payload_len_block, 1);

    if ((mask_payload_len_block & (~0x80)) == 0x7e)
    {
        int16_t extended_payload_len_16 = boost::endian::native_to_big((int16_t)payload_len);
        memmove(out_buffer + shift, &extended_payload_len_16, 2);
        shift += 2;
    }
    else if ((mask_payload_len_block & (~0x80)) == 0x7f)
    {
        int64_t extended_payload_len_64 = boost::endian::native_to_big((int64_t)payload_len);
        memmove(out_buffer + shift, &extended_payload_len_64, 8);
        shift += 8;
    }

    if (use_mask)
    {
        memmove(out_buffer + shift, masking_key.data(), 4);
        shift += 4;
    }

    return shift; // same as length of header
}

uint32_t data_frame_header::binary_pack_size() const
{
    // FOLLOWING PROCESS as same as binary_pack() method, but This function doesn't call memmove()
    uint32_t shift = 0;

    //x int8_t fin_opcode_block = (int8_t)0x1; // must be binary frame (net_stream interface requirement)
    //x (fin) ? (fin_opcode_block |= 0x80) : (fin_opcode_block |= 0x00);
    //x memmove(out_buffer + shift++, &fin_opcode_block, 1);
    shift += 1;

    int8_t mask_payload_len_block = 0;
    if (payload_len >= 0x7e)
    {
        if (payload_len <= INT16_MAX)
            mask_payload_len_block = 0x7e;
        else
            mask_payload_len_block = 0x7f;
    }
    else
        mask_payload_len_block = (int8_t)payload_len;
    (use_mask) ? (mask_payload_len_block |= 0x80) : (mask_payload_len_block |= 0x00);
    //x memmove(out_buffer + shift++, &mask_payload_len_block, 1);
    shift += 1;

    if ((mask_payload_len_block & (~0x80)) == 0x7e)
    {
        int16_t extended_payload_len_16 = boost::endian::native_to_big((int16_t)payload_len);
        //x memmove(out_buffer + shift, &extended_payload_len_16, 2);
        shift += 2;
    }
    else if ((mask_payload_len_block & (~0x80)) == 0x7f)
    {
        int64_t extended_payload_len_64 = boost::endian::native_to_big((int64_t)payload_len);
        //x memmove(out_buffer + shift, &extended_payload_len_64, 8);
        shift += 8;
    }

    if (use_mask)
    {
        //x memmove(out_buffer + shift, masking_key.data(), 4);
        shift += 4;
    }

    return shift; // same as length of header
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
    ::std::shared_ptr<tcp_stream> raw_tcp_stream = ::std::static_pointer_cast<tcp_stream>(_tcp_server.accept());
    bool handshake_result = _handshake(*raw_tcp_stream);
    if (handshake_result == true)
    {
        ::std::shared_ptr<net_stream> stream = ::std::make_shared<websocket_stream>( static_cast<tcp_stream&&>(*raw_tcp_stream) );
        return stream;
    }
    else
    {
        throw vee::exception("rfc6455_client_handshake_failure", (int)error_code::rfc6455_handshake_failure);
    }
}

void websocket_server::async_accept(std::function<_vee_net_async_accept_callback_sig> e)
{
    auto handle_accept = [e](operation_result& result, ::std::shared_ptr<net_stream>& stream)
    {
        if (result.error != error_code::success)
        {
            e(result, stream);
        }
        else
        {
            ::std::shared_ptr<tcp_stream> raw_tcp_stream = ::std::static_pointer_cast<tcp_stream>(stream);
            bool handshake_result = websocket_server::_handshake(*raw_tcp_stream);
            if (!handshake_result)
            {
                result.error = error_code::rfc6455_handshake_failure;
                result.desc.append("\r\nrfc6455_client_handshake_failure!");
            }
            else
            {
                ::std::shared_ptr<net_stream> stream = ::std::make_shared<websocket_stream>(static_cast<tcp_stream&&>(*raw_tcp_stream));
                e(result, stream);
            }
        }
    };
    _tcp_server.async_accept(handle_accept);
}

bool websocket_server::_handshake(net_stream& raw_socket)
{
    try
    {
        ::std::array<byte, 4096> buffer;
        raw_socket.read(buffer.data(), buffer.size());
        
        // Parsing HTTP header
        handshake_client_request header;
        header.parse(make_string((char*)buffer.data()));
        
        // Validating the client header
        if (header.is_valid() == false)
        {
            return false;
        }

        printf("websocket_server> [RFC6455 Handshake] Websocket client request\n");
        PRINT_LINE;
        header.print();
        PRINT_LINE;

        // Append magic GUID
        string magic_string(header.sec_websocket_key);
        magic_string.append(RFC4122_GUID::get());
        // Hashing via SHA-1
        auto hash_code = sha1_hashing(magic_string);
        print_hash_code(hash_code);
        // Encoding via Base64
        string hash_code_base64 = base64::encode(hash_code);
        printf("To base64: %s\n", hash_code_base64.data());
        // Make data for response
        handshake_server_response response(hash_code_base64);
        string response_data = response.binary_pack();

        printf("websocket_server> [RFC6455 Handshake] Websocket server response\n");
        PRINT_LINE;
        puts(response_data.data());
        PRINT_LINE;
        // Send response data to client
        raw_socket.write((byte*)response_data.data(), response_data.size());
    }
    catch (vee::exception& e)
    {
        printf("websocket_server> exception occured at _handshake() %s\n", e.what());
        return false;
    }
    return true;
}

websocket_stream::websocket_stream():
_tcp_stream(io_service_sigleton::get().io_service()),
_host_io_service_ptr(&(_tcp_stream.get_io_service()))
{

}

websocket_stream::websocket_stream(tcp_stream&& stream):
_tcp_stream(static_cast<tcp_stream&&>(stream)),
_host_io_service_ptr(&(_tcp_stream.get_io_service()))
{

}

websocket_stream::~websocket_stream()
{

}

websocket_stream& websocket_stream::operator=(websocket_stream&& rhs)
{
    _tcp_stream = static_cast<tcp_stream&&>(rhs._tcp_stream);
    _host_io_service_ptr = &(_tcp_stream.get_io_service());
    return *this;
}

void websocket_stream::connect(const char* ip_addr, port_t port) throw(...)
{
#pragma warning(disable:4996)
    handshake_client_request request;
    request.request_uri = "GET /chat HTTP/1.1";
    request.host = "127.0.0.1:";
    {
        ::std::array<char, 128> buffer;
        sprintf(buffer.data(), "%d", port);
        request.host.append(buffer.data());
    }
    request.upgrade = "websocket";
    request.connection = "Upgrade";
    request.sec_websocket_key = "dGhlIHNhbXBsZSBub25jZQ=="; //TODO: Randomize
    request.origin = "http://127.0.0.1";
    {
        ::std::array<char, 128> buffer;
        sprintf(buffer.data(), "%d", port);
        request.origin.append(buffer.data());
    }
    request.sec_websocket_protocol = "char, superchat";
    request.sec_web_socket_version = "13";

    handshake_server_response response;
    ::std::array<char, 512> response_buffer;
    {
        string pack = request.binary_pack();
        _tcp_stream.connect(ip_addr, port);
        _tcp_stream.write((byte*)pack.data(), pack.size());
        _tcp_stream.read((byte*)response_buffer.data(), response_buffer.size());
    }
    response.parse(response_buffer.data());
    //Temporary validate check process
    if (response.sec_websocket_accept.compare("s3pPLMBiTxaQ9kYGzzhZRbK+xOo=") != 0)
    { // indvlid response
        throw ::vee::exception("Sec-Websocket-Accept key is wrong!", (int)error_code::rfc6455_handshake_failure);
    }
    printf("Connect success!");
#pragma warning(default:4996)
}

void websocket_stream::async_connect(const char* ip_addr, port_t port, async_connect_callback e)
{
    _tcp_stream.async_connect(ip_addr, port, e);
}

void websocket_stream::disconnect()
{
    std::array<unsigned char, 128> buffer;
    buffer.fill(0);
    write(opcode_id::connnection_close, buffer.data(), buffer.size());
    _tcp_stream.disconnect();
}

ws_stream::io_result websocket_stream::write(opcode_id opcode, const byte* data, const uint32_t len) throw(...)
{
    auto ws_packet = _convert_to_websocket_form(opcode, data, len);
    _tcp_stream.write(ws_packet.second.data(), ws_packet.second.size());
    return ws_packet.first; // return io_result
}

void websocket_stream::async_write(opcode_id opcode, const byte* data, const uint32_t len, async_write_callback e) throw(...)
{
    auto packet = _convert_to_websocket_form(opcode, data, len);
    _tcp_stream.async_write(packet.second.data(), packet.first.header_size + len, std::move(e));
}

ws_stream::io_result websocket_stream::read_all(byte* const buffer, const uint32_t buf_capacity) throw(...)
{
    while (true)
    {
        try
        {
            uint32_t bytes_transferred = _tcp_stream.read(buffer, buf_capacity);
            auto info = _preprocess_received_data(buffer, bytes_transferred);
            return info.first;
        }
        catch (vee::exception& e)
        {
            if (e.code == (int)error_code::rfc6455_websocket_heartbeat)
            {
                continue;
            }
            else
            {
                throw e;
            }
        }
    }
}

ws_stream::io_result websocket_stream::read_payload_only(byte* const buffer, const uint32_t buf_capacity) throw(...)
{
    io_result result = read_all(buffer, buf_capacity);
    memmove(buffer, buffer + result.header_size, (uint32_t)(result.payload_size));
    memset(buffer + result.payload_size, 0, (uint32_t)(buf_capacity - result.payload_size));
    return result;
}

void websocket_stream::async_read_payload_only(byte* const buffer, const uint32_t buf_capacity, async_read_callback e) throw(...)
{
    async_read_callback preprocess = [&preprocess, this, e](operation_result& function_result, byte* const buffer, const uint32_t buf_capacity, const uint32_t bytes_transferred) -> void
    {
        if (function_result.error != error_code::success)
        {
            e(function_result, buffer, buf_capacity, bytes_transferred);
        }
        try
        {
            auto info = _preprocess_received_data(buffer, bytes_transferred);
            memmove(buffer, buffer + info.first.header_size, (uint32_t)(info.first.payload_size));
            memset(buffer + info.first.payload_size, 0, (uint32_t)(buf_capacity - info.first.payload_size));
            e(function_result, buffer, buf_capacity, info.first.payload_size);
        }
        catch (vee::exception& ex)
        {
            if (ex.code == (int)error_code::rfc6455_websocket_heartbeat)
            {
                _tcp_stream.async_read(buffer, buf_capacity, preprocess);
            }
            else
            {
                throw ex;
            }
        }
    };
    _tcp_stream.async_read(buffer, buf_capacity, preprocess);
}

void websocket_stream::async_read_all(byte* const buffer, const uint32_t buf_capacity, async_read_callback e) throw(...)
{
    async_read_callback preprocess = [&preprocess, this, e](operation_result& function_result, byte* const buffer, const uint32_t buf_capacity, const uint32_t bytes_transferred) -> void
    {
        if (function_result.error != error_code::success)
        {
            e(function_result, buffer, buf_capacity, bytes_transferred);
        }
        try
        {
            auto info = _preprocess_received_data(buffer, bytes_transferred);
            e(function_result, buffer, buf_capacity, bytes_transferred);
        }
        catch (vee::exception& ex)
        {
            if (ex.code == (int)error_code::rfc6455_websocket_heartbeat)
            {
                _tcp_stream.async_read(buffer, buf_capacity, preprocess);
            }
            else
            {
                throw ex;
            }
        }
    };
    _tcp_stream.async_read(buffer, buf_capacity, preprocess);
}

void websocket_stream::conversion(tcp_stream&& stream)
{
    _tcp_stream = static_cast<tcp_stream&&>(stream);
    _host_io_service_ptr = &(_tcp_stream.get_io_service());
}

::std::shared_ptr<net_server> create_server(unsigned short port)
{
    ::std::shared_ptr<net_server> server = ::std::make_shared<websocket_server>(port);
    return server;
}

std::pair<ws_stream::io_result /*header and payload size*/, std::vector<byte> /*data*/> websocket_stream::_convert_to_websocket_form(opcode_id opcode, const byte* data, const uint32_t len)
{
    data_frame_header header;
    header.fin = true; //TODO: 쪼개서 보내는 함수 지원하기, 지금은 무조건 한번에 다! 보낸다! 스펙상 INT64_MAX만큼 한번에 보낼 수 있지만 브라우저가 뻗을 듯
    header.use_mask = false; //TODO: 커스텀 마스크 사용하는 함수 지원하기, 지금은 고정된 마스크 -> 클라이언트에만 필요, 서버는 절대 마스킹하면 안됨!
    header.payload_len = len;
    uint32_t header_size = header.binary_pack_size();
    std::vector<byte> packet_data((uint32_t)(len + header_size), 0); //TODO: 매번 벡터를 안만드는 방법을 생각해보자.
    memmove(packet_data.data() + header.binary_pack(opcode, packet_data.data()), data, (uint32_t)len);

    std::pair<ws_stream::io_result /*header and payload size*/, std::vector<byte> /*data*/> ret;
    ret.first.header_size = header_size;
    ret.first.payload_size = (uint32_t)header.payload_len;
    ret.second = std::move(packet_data);

    return ret;
}

std::pair<ws_stream::io_result /*header and payload size*/, data_frame_header /*header*/> websocket_stream::_preprocess_received_data(byte* const data, const uint32_t len) throw(...)
{
    //TODO: FIN 패킷이 아닐 때 인터페이스에 맞춰주는 코드.... 필요할까?
    std::pair<ws_stream::io_result /*header and payload size*/, data_frame_header /*header*/> ret;
    uint32_t bytes_transferred = len;
    io_result& io_result = ret.first;
    data_frame_header& header = ret.second;

    header.analyze(data, bytes_transferred);
    switch (header.opcode)
    {
    case opcode_id::ping:
    case opcode_id::pong:
            throw vee::exception("Websocket heartbeat", (int)error_code::rfc6455_websocket_heartbeat);
    default:
        break;
    }

    // Check the connection close operation
    if (header.opcode == opcode_id::connnection_close)
    {
        throw ::vee::exception("Connection is closed by host", (int)error_code::stream_disconnected_by_host);
    }

    // Copy binary data from buffer (DO NOT USE EXTEISNION FLAG!)
    // Unmask
    {
        for (unsigned int i = 0; i < header.payload_len; ++i)
        {
            unsigned char mask = header.masking_key[i % 4];
            data[i + header.payload_pos] ^= mask;
        }
    }

    ret.first.header_size = bytes_transferred - (uint32_t)header.payload_len;
    ret.first.payload_size = (uint32_t)header.payload_len;

    return ret;
}

::std::shared_ptr<ws_stream> create_stream()
{
    //TODO: IO_SERVICE SELECTION
    ::std::shared_ptr<ws_stream> stream = ::std::make_shared<websocket_stream>();
    return stream;
}

} // namespace ws
} // namespace net
} // namespace voost
} // namespace vee