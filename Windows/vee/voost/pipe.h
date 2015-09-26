#ifndef _VEE_VOOST_PIPE_H_
#define _VEE_VOOST_PIPE_H_

#include <vee/stream.h>

namespace vee {
namespace voost {
namespace interprocess {

struct pipe_time_out_constant
{
    static const int wait_default = 0x00000000;
    static const int wait_forever = 0xffffffff;
    static const int wait_nowait  = 0x00000001;
};

enum class creation_option: int
{
    create_or_truncate, // exists -> truncates, does not exist -> creates
    create_new,         // exists -> fails, does not exist -> creates
    open_or_crate,      // exists -> opens, does not exist -> creates
    open_existing,      // exists -> opens, does not exist -> fails
    truncate_existing   // exists -> truncates, does not eixst -> fails
};

enum class pipe_data_transfer_mode: int
{
    iomode_byte,      // Data is read from pipe as a stream of bytes,  This mode is the default if no read-mode flag is specified.
    iomode_message,   // Data is read from the pipe as a stream of messages. The function fails if this flag is specified for a byte-type pipe.
};

class named_pipe abstract: public sync_stream
{
public:
    virtual ~named_pipe() = default;
    virtual void connect(const char* pipe_name, const creation_option creation_opt, const pipe_data_transfer_mode read_mode, const uint32_t time_out_millisec) throw(...) = 0;
};

class named_pipe_server abstract
{
public:
    using session_t = named_pipe;
    using session_ptr = ::std::shared_ptr<session_t>;
    virtual ~named_pipe_server() = default;
    virtual session_ptr accept(const string pipe_name, const pipe_data_transfer_mode transfer_mode) throw(...) = 0;
    virtual void close() __noexcept = 0;
};

} // namespace interprocess
} // namespace voost
} // namespace vee

#endif // !_VEE_VOOST_PIPE_H_
