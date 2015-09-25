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

enum class pipe_read_mode: int
{
    readmode_byte,      // Data is read from pipe as a stream of bytes,  This mode is the default if no read-mode flag is specified.
    readmode_message,   // Data is read from the pipe as a stream of messages. The function fails if this flag is specified for a byte-type pipe.
};

class named_pipe abstract: public sync_stream
{
public:
    virtual ~named_pipe() = default;
    virtual void connect(const char* pipe_name, const creation_option creation_opt, const pipe_read_mode read_mode, const uint32_t time_out_millisec) throw(...) = 0;
};

} // namespace interprocess
} // namespace voost
} // namespace vee

#endif // !_VEE_VOOST_PIPE_H_
