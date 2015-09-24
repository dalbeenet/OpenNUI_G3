#include <vee/voost/net.h>

namespace vee {
namespace voost {
namespace net {

void net_stream::async_connect(const char* ip_addr, port_t port, async_connect_delegate& e)
{
    return async_connect(ip_addr, port, std::make_shared<async_connect_delegate>(e));
}

void net_stream::async_connect(const char* ip_addr, port_t port, async_connect_delegate&& e)
{
    return async_connect(ip_addr, port, std::make_shared<async_connect_delegate>(std::move(e)));
}

void net_stream::async_connect(const char* ip_addr, port_t port, async_connect_delegate_ptr& e_ptr)
{
    auto caller = [e_ptr](operation_result& result) -> void
    {
        e_ptr->operator()(result);
    };
    return async_connect(ip_addr, port, caller);
}

void net_stream::async_connect(const char* ip_addr, port_t port, async_connect_delegate_ptr&& e_ptr)
{
    auto caller = [e_ptr](operation_result& result) -> void
    {
        e_ptr->operator()(result);
    };
    return async_connect(ip_addr, port, caller);
}

void net_server::async_accept(async_accept_delegate_t e)
{
    return async_accept(std::make_shared<async_accept_delegate_t>(e));
}

void net_server::async_accept(std::shared_ptr<async_accept_delegate_t> e)
{
    auto caller = [e](operation_result& result, ::std::shared_ptr<net_stream> stream) -> void
    {
        e->operator()(result, stream);
    };
    return async_accept(caller);
}

namespace websocket {

uint32_t websocket_stream::write(const byte* data, uint32_t len) throw(...)
{
    io_result result = write(opcode_id::binary_frame, data, len);
    return result.header_size + result.payload_size;
}

void websocket_stream::async_write(const byte* data, uint32_t len, async_write_callback e) throw(...)
{
    return async_write(opcode_id::binary_frame, data, len, e);
}

void websocket_stream::async_write(opcode_id opcode, const byte* data, uint32_t len, async_write_delegate& e) throw(...)
{
    return async_write(opcode, data, len, std::make_shared<async_write_delegate>(e));
}

void websocket_stream::async_write(opcode_id opcode, const byte* data, uint32_t len, async_write_delegate&& e) throw(...)
{
    return async_write(opcode, data, len, std::make_shared<async_write_delegate>(std::move(e)));
}

void websocket_stream::async_write(opcode_id opcode, const byte* data, uint32_t len, async_write_delegate_ptr& e_ptr) throw(...)
{
    auto caller = [e_ptr](operation_result& result, uint32_t bytes_transferred) -> void
    {
        e_ptr->operator()(result, bytes_transferred);
    };
    return async_write(opcode_id::binary_frame, data, len, caller);
}

void websocket_stream::async_write(opcode_id opcode, const byte* data, uint32_t len, async_write_delegate_ptr&& e_ptr) throw(...)
{
    auto caller = [e_ptr](operation_result& result, uint32_t bytes_transferred) -> void
    {
        e_ptr->operator()(result, bytes_transferred);
    };
    return async_write(opcode_id::binary_frame, data, len, caller);
}

uint32_t websocket_stream::read(byte* const buffer, uint32_t buf_capacity) throw(...)
{
    return read_payload_only(buffer, buf_capacity).payload_size;
}

void websocket_stream::async_read(byte* const buffer, uint32_t buf_capacity, async_read_callback e) throw(...)
{
    return async_read_payload_only(buffer, buf_capacity, e);
}

void websocket_stream::async_read_payload_only(byte* const buffer, uint32_t buf_capacity, async_read_delegate& e) throw(...)
{
    return async_read_payload_only(buffer, buf_capacity, std::make_shared<async_read_delegate>(e));
}

void websocket_stream::async_read_payload_only(byte* const buffer, uint32_t buf_capacity, async_read_delegate&& e) throw(...)
{
    return async_read_payload_only(buffer, buf_capacity, std::make_shared<async_read_delegate>(std::move(e)));
}

void websocket_stream::async_read_payload_only(byte* const buffer, uint32_t buf_capacity, async_read_delegate_ptr& e_ptr) throw(...)
{
    auto caller = [e_ptr](operation_result& result, byte* const buffer, uint32_t buf_capacity, uint32_t bytes_transferred) -> void
    {
        e_ptr->operator()(result, buffer, buf_capacity, bytes_transferred);
    };
    return async_read_payload_only(buffer, buf_capacity, caller);
}

void websocket_stream::async_read_payload_only(byte* const buffer, uint32_t buf_capacity, async_read_delegate_ptr&& e_ptr) throw(...)
{
    auto caller = [e_ptr](operation_result& result, byte* const buffer, uint32_t buf_capacity, uint32_t bytes_transferred) -> void
    {
        e_ptr->operator()(result, buffer, buf_capacity, bytes_transferred);
    };
    return async_read_payload_only(buffer, buf_capacity, caller);
}

void websocket_stream::async_read_all(byte* const buffer, uint32_t buf_capacity, async_read_delegate& e) throw(...)
{
    return async_read_all(buffer, buf_capacity, std::make_shared<async_read_delegate>(e));
}

void websocket_stream::async_read_all(byte* const buffer, uint32_t buf_capacity, async_read_delegate&& e) throw(...)
{
    return async_read_all(buffer, buf_capacity, std::make_shared<async_read_delegate>(std::move(e)));
}

void websocket_stream::async_read_all(byte* const buffer, uint32_t buf_capacity, async_read_delegate_ptr& e) throw(...)
{
    auto caller = [e](operation_result& result, byte* const buffer, uint32_t buf_capacity, uint32_t bytes_transferred) -> void
    {
        e->operator()(result, buffer, buf_capacity, bytes_transferred);
    };
    return async_read_all(buffer, buf_capacity, caller);
}

void websocket_stream::async_read_all(byte* const buffer, uint32_t buf_capacity, async_read_delegate_ptr&& e) throw(...)
{
    auto caller = [e](operation_result& result, byte* const buffer, uint32_t buf_capacity, uint32_t bytes_transferred) -> void
    {
        e->operator()(result, buffer, buf_capacity, bytes_transferred);
    };
    return async_read_all(buffer, buf_capacity, caller);
}

} // namespace websocket

} // namespace net
} // namespace voost
} // namespace vee