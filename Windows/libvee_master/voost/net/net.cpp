#include <vee/voost/net.h>

namespace vee {
namespace voost {
namespace net {

void net_stream::async_connect(const char* ip_addr, port_t port, async_connect_delegate_t e)
{
    return async_connect(ip_addr, port, std::make_shared<async_connect_delegate_t>(e));
}

void net_stream::async_connect(const char* ip_addr, port_t port, std::shared_ptr<async_connect_delegate_t> e)
{
    auto caller = [e](op_result& result) -> void
    {
    e->operator()(result);
    };
    return async_connect(ip_addr, port, caller);
}

void net_stream::async_write(const byte* data, net::size_t len, async_write_delegate_t e) throw(...)
{
    return async_write(data, len, std::make_shared<async_write_delegate_t>(e));
}

void net_stream::async_write(const byte* data, net::size_t len, std::shared_ptr<async_write_delegate_t> e) throw(...)
{
    auto caller = [e](op_result& result, net::size_t bytes_transferred) -> void
    {
    e->operator()(result, bytes_transferred);
    };
    return async_write(data, len, caller);
}

void net_stream::async_read(byte* const buffer, net::size_t buf_capacity, async_read_delegate_t e) throw(...)
{
    return async_read(buffer, buf_capacity, std::make_shared<async_read_delegate_t>(e));
}

void net_stream::async_read(byte* const buffer, net::size_t buf_capacity, std::shared_ptr<async_read_delegate_t> e) throw(...)
{
    auto caller = [e]( op_result& result, byte* const buffer, net::size_t buf_capacity, net::size_t bytes_transferred) -> void
    {
        e->operator()(result, buffer, buf_capacity, bytes_transferred);
    };
    return async_read(buffer, buf_capacity, caller);
}

namespace websocket {

net::size_t ws_stream::write(const byte* data, net::size_t len) throw(...)
{
    io_result result = write(opcode_id::binary_frame, data, len);
    return result.header_size + result.payload_size;
}

void ws_stream::async_write(const byte* data, net::size_t len, std::function<_vee_net_async_write_callback_sig> e) throw(...)
{
    return async_write(opcode_id::binary_frame, data, len, e);
}

void ws_stream::async_write(opcode_id opcode, const byte* data, net::size_t len, std::shared_ptr<async_write_delegate_t> e) throw(...)
{
    auto caller = [e](op_result& result, net::size_t bytes_transferred) -> void
    {
        e->operator()(result, bytes_transferred);
    };
    return async_write(opcode_id::binary_frame, data, len, caller);
}

net::size_t ws_stream::read(byte* const buffer, net::size_t buf_capacity) throw(...)
{
    return read_payload_only(buffer, buf_capacity).payload_size;
}

void ws_stream::async_read(byte* const buffer, net::size_t buf_capacity, std::function<_vee_net_async_read_callback_sig> e) throw(...)
{
    return async_read_payload_only(buffer, buf_capacity, e);
}

void ws_stream::async_read_payload_only(byte* const buffer, net::size_t buf_capacity, std::shared_ptr<async_read_delegate_t> e) throw(...)
{
    auto caller = [e](op_result& result, byte* const buffer, net::size_t buf_capacity, net::size_t bytes_transferred) -> void
    {
    e->operator()(result, buffer, buf_capacity, bytes_transferred);
    };
    return async_read_payload_only(buffer, buf_capacity, caller);
}

void ws_stream::async_read_all(byte* const buffer, net::size_t buf_capacity, std::shared_ptr<async_read_delegate_t> e) throw(...)
{
    auto caller = [e](op_result& result, byte* const buffer, net::size_t buf_capacity, net::size_t bytes_transferred) -> void
    {
        e->operator()(result, buffer, buf_capacity, bytes_transferred);
    };
    return async_read_all(buffer, buf_capacity, caller);
}

} // namespace websocket

} // namespace net
} // namespace voost
} // namespace vee