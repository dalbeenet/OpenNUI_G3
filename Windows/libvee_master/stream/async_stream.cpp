#include <vee/stream.h>

namespace vee {

void async_stream::async_read(byte* const buffer, const uint32_t buf_capacity, const async_read_delegate& e) throw(...)
{
    return this->async_read(buffer, buf_capacity, ::std::make_shared<async_read_delegate>(e));
}

void async_stream::async_read(byte* const buffer, const uint32_t buf_capacity, async_read_delegate&& e) throw(...)
{
    return this->async_read(buffer, buf_capacity, ::std::make_shared<async_read_delegate>(std::move(e)));
}

void async_stream::async_read(byte* const buffer, const uint32_t buf_capacity, async_read_delegate_ptr& e_ptr) throw(...)
{
    auto caller = [e_ptr](operation_result& op_result, byte* const buffer, uint32_t buf_capacity, uint32_t byte_transferred) -> void
    {
        e_ptr->operator()(op_result, buffer, buf_capacity, byte_transferred);
    };
    return this->async_read(buffer, buf_capacity, caller);
}

void async_stream::async_read(byte* const buffer, const uint32_t buf_capacity, async_read_delegate_ptr&& e_ptr) throw(...)
{
    auto caller = [e_ptr](operation_result& op_result, byte* const buffer, uint32_t buf_capacity, uint32_t byte_transferred) -> void
    {
        e_ptr->operator()(op_result, buffer, buf_capacity, byte_transferred);
    };
    return this->async_read(buffer, buf_capacity, caller);
}

void async_stream::async_write(const byte* data, const uint32_t len, const async_write_delegate& e) throw(...)
{
    return this->async_write(data, len, ::std::make_shared<async_write_delegate>(e));
}

void async_stream::async_write(const byte* data, const uint32_t len, async_write_delegate&& e) throw(...)
{
    return this->async_write(data, len, ::std::make_shared<async_write_delegate>(::std::move(e)));
}

void async_stream::async_write(const byte* data, const uint32_t len, async_write_delegate_ptr& e_ptr) throw(...)
{
    auto caller = [e_ptr](operation_result& op_result, uint32_t bytes_transferred)
    {
        e_ptr->operator()(op_result, bytes_transferred);
    };
    return this->async_write(data, len, caller);
}

void async_stream::async_write(const byte* data, const uint32_t len, async_write_delegate_ptr&& e_ptr) throw(...)
{
    auto caller = [e_ptr](operation_result& op_result, uint32_t bytes_transferred)
    {
        e_ptr->operator()(op_result, bytes_transferred);
    };
    return this->async_write(data, len, caller);
}

} // namespace vee