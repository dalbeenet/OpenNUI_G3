#include <vee/voost/pipe.h>
#include <vee/macro.h>
#include <vee/exception.h>
#include <vee/win32.h>
#include <vee/voost/asio.h>

namespace vee {
namespace voost {
namespace interprocess {
namespace win32 {

class win32_named_pipe: public named_pipe
{
    DISALLOW_COPY_AND_ASSIGN(win32_named_pipe);
public:
    using asio_stream_handle = ::boost::asio::windows::stream_handle;
    virtual ~win32_named_pipe() __noexcept;
    win32_named_pipe() __noexcept;
    win32_named_pipe(HANDLE handle, const char* pipe_name, bool is_server_side);
    win32_named_pipe(asio_stream_handle&& handle, const char* pipe_name, bool is_server_side) __noexcept;
    win32_named_pipe(win32_named_pipe&& other) __noexcept;
    win32_named_pipe& operator=(win32_named_pipe&& other) __noexcept;
    virtual void connect(const char* pipe_name,
                         const creation_option creation_opt,
                         const pipe_data_transfer_mode read_mode,
                         const uint32_t time_out_millisec) throw(...) override;
    virtual void disconnect() __noexcept override;
    virtual uint32_t write(const byte* data, const uint32_t size) throw(...) override;
    virtual void async_write(const byte* data, const uint32_t len, async_write_callback e) throw(...) override;
    virtual uint32_t read(byte* const buffer, const uint32_t buf_capacity) throw(...) override;
    virtual void async_read(byte* const buffer, const uint32_t buf_capacity, async_read_callback e) throw(...) override;
    inline string& pipe_name()
    {
        return _pipe_name;
    }
protected:
    string _pipe_name;
    asio_stream_handle _stream_handler;
    bool   _is_server_side;
};

class win32_named_pipe_acceptor
{
    DISALLOW_COPY_AND_ASSIGN(win32_named_pipe_acceptor);
public:
    ~win32_named_pipe_acceptor() __noexcept;
    win32_named_pipe_acceptor() __noexcept;
    win32_named_pipe_acceptor(win32_named_pipe_acceptor&& other) __noexcept;
    win32_named_pipe_acceptor& operator=(win32_named_pipe_acceptor&& other) __noexcept;
    win32_named_pipe accept(const char* pipe_name,
                            const pipe_data_transfer_mode mode,
                            const uint32_t in_buffer_size,
                            const uint32_t out_buffer_size) throw(...);
};

class win32_named_pipe_server: public named_pipe_server
{
    DISALLOW_COPY_AND_ASSIGN(win32_named_pipe_server);
public:
    using session_t = win32_named_pipe;
    using session_ptr = ::std::shared_ptr<session_t>;
    using generic_session_ptr = ::std::shared_ptr<named_pipe>;
    virtual ~win32_named_pipe_server() __noexcept;
    win32_named_pipe_server() __noexcept;
    win32_named_pipe_server(win32_named_pipe_server&&) __noexcept;
    win32_named_pipe_server& operator=(win32_named_pipe_server&&)__noexcept;
    virtual generic_session_ptr     accept(const char* pipe_name,
                                           const pipe_data_transfer_mode mode,
                                           const uint32_t in_buffer_size,
                                           const uint32_t out_buffer_size) throw(...) override;
    virtual void close() __noexcept override;
protected:
    win32_named_pipe_acceptor _acceptor;
}; 

} // namespace win32
} // namespace interprocess
} // namespace voost
} // namespace vee