#include <vee/voost/pipe.h>
#include <vee/macro.h>
#include <vee/exception.h>
#include <vee/win32.h>

namespace vee {
namespace voost {
namespace interprocess {

class win32_named_pipe: named_pipe
{
    DISALLOW_COPY_AND_ASSIGN(win32_named_pipe);
public:
    ~win32_named_pipe() __noexcept;
    win32_named_pipe() __noexcept;
    win32_named_pipe(win32_handle&& handle) __noexcept;
    win32_named_pipe(win32_named_pipe&& other) __noexcept;
    win32_named_pipe& operator=(win32_named_pipe&& other) __noexcept;
    virtual void connect(const char* pipe_name, const creation_option creation_opt, const pipe_data_transfer_mode read_mode, const uint32_t time_out_millisec) throw(...) override;
    virtual uint32_t write(const byte* data, const uint32_t size) throw(...) override;
    virtual uint32_t read(byte* const buffer, const uint32_t buf_capacity) throw(...) override;
    inline string& pipe_name()
    {
        return _pipe_name;
    }
protected:
    win32_handle _pipe_handle;
    string _pipe_name;
};

class win32_named_pipe_acceptor
{
    DISALLOW_COPY_AND_ASSIGN(win32_named_pipe_acceptor);
public:
    ~win32_named_pipe_acceptor() __noexcept;
    win32_named_pipe_acceptor() __noexcept;
    win32_named_pipe_acceptor(win32_named_pipe_acceptor&& other) __noexcept;
    win32_named_pipe_acceptor& operator=(win32_named_pipe_acceptor&& other) __noexcept;
    win32_named_pipe accept(const string pipe_name, const pipe_data_transfer_mode mode, const uint32_t in_buffer_size, const uint32_t out_buffer_size) throw(...);
};

} // namespace interprocess
} // namespace voost
} // namespace vee