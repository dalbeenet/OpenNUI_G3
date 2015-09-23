#ifndef _VEE_STREAM_H_
#define _VEE_STREAM_H_

#include <cinttypes>
#include <memory>
#include <vee/system.h>
#include <vee/delegate.h>

namespace vee {

using byte = unsigned char;
using operation_result = ::vee::system::operation_result;
using error_code = ::vee::system::error_code;

class sync_stream abstract
{
public:
    virtual ~sync_stream() = default;
    virtual uint32_t write(const byte* data, const uint32_t size) throw(...) = 0;
    virtual uint32_t read(byte* const buffer, const uint32_t buf_capacity) throw(...) = 0;
};

#define _vee_async_read_callback_sig void(::vee::system::operation_result& /*opereation_result*/, ::vee::byte* const /*recieve_buffer_address*/, uint32_t /*recieve_buffer_size*/, uint32_t /*bytes_transferred*/)
#define _vee_async_write_callback_sig void(::vee::system::operation_result& /*opereation_result*/, uint32_t /*bytes_transferred*/)
class async_stream abstract
{
public:
    // local types
    using async_read_delegate      = ::vee::delegate<_vee_async_read_callback_sig>;
    using async_read_delegate_ptr  = ::std::shared_ptr<async_read_delegate>;
    using async_read_callback      = ::std::function<_vee_async_read_callback_sig>;
    using async_write_delegate     = ::vee::delegate<_vee_async_write_callback_sig>;
    using async_write_delegate_ptr = ::std::shared_ptr<async_write_delegate>;
    using async_write_callback     = ::std::function<_vee_async_write_callback_sig>;
    
    // public virtual member functions
    virtual ~async_stream() = default;
    virtual void async_read(byte* const buffer, const uint32_t buf_capacity, const async_read_delegate& e) throw(...);
    virtual void async_read(byte* const buffer, const uint32_t buf_capacity, async_read_delegate&& e) throw(...);
    virtual void async_read(byte* const buffer, const uint32_t buf_capacity, async_read_delegate_ptr& e_ptr) throw(...);
    virtual void async_read(byte* const buffer, const uint32_t buf_capacity, async_read_delegate_ptr&& e_ptr) throw(...);
    virtual void async_write(const byte* data, const uint32_t len, const async_write_delegate& e) throw(...);
    virtual void async_write(const byte* data, const uint32_t len, async_write_delegate&& e) throw(...);
    virtual void async_write(const byte* data, const uint32_t len, async_write_delegate_ptr& e_ptr) throw(...);
    virtual void async_write(const byte* data, const uint32_t len, async_write_delegate_ptr&& e_ptr) throw(...);

    // public pure virtual functions
    virtual void async_read(byte* const buffer, const uint32_t buf_capacity, async_read_callback e) throw(...) = 0;
    virtual void async_write(const byte* data, const uint32_t len, async_write_callback e) throw(...) = 0;
};

class iostream abstract: public sync_stream, public async_stream
{
public:
};

};

#endif // !_VEE_STREAM_H_
