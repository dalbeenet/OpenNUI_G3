#include <vee/voost/net.h>

namespace vee {
namespace voost {
namespace net {

void net_stream::async_connect(const char* ip_addr, port_t port, async_connect_callback e)
{
    return async_connect(ip_addr, port, std::make_shared<async_connect_callback>(e));
}

void net_stream::async_write(const byte* data, net::size_t len, async_write_callback e) throw(...)
{
    return async_write(data, len, std::make_shared<async_write_callback>(e));
}

void net_stream::async_read(byte* const buffer, net::size_t buf_capacity, async_read_callback e) throw(...)
{
    return async_read(buffer, buf_capacity, std::make_shared<async_read_callback>(e));
}

namespace websocket {

net::size_t ws_stream::write(const byte* data, net::size_t len) throw(...)
{
    io_result result = write(opcode_id::binary_frame, data, len);
    return result.header_size + result.payload_size;
}

void ws_stream::async_write(const byte* data, net::size_t len, std::shared_ptr<async_write_callback> e) throw(...)
{
    return async_write(opcode_id::binary_frame, data, len, e);
}

net::size_t ws_stream::read(byte* const buffer, net::size_t buf_capacity) throw(...)
{
    return read_payload_only(buffer, buf_capacity).payload_size;
}

void ws_stream::async_read(byte* const buffer, net::size_t buf_capacity, std::shared_ptr<async_read_callback> e) throw(...)
{
    return async_read_payload_only(buffer, buf_capacity, e);
}

} // namespace websocket

} // namespace net
} // namespace voost
} // namespace vee