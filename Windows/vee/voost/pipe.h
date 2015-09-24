#ifndef _VEE_VOOST_PIPE_H_
#define _VEE_VOOST_PIPE_H_

#include <vee/stream.h>

namespace vee {
namespace voost {
namespace interprocess {

enum class creation_option: int
{
    create_or_truncate, // exists -> truncates, does not exist -> creates
    create_new,         // exists -> fails, does not exist -> creates
    open_or_crate,      // exists -> opens, does not exist -> creates
    open_existing,      // exists -> opens, does not exist -> fails
    truncate_existing   // exists -> truncates, does not eixst -> fails
};

class named_pipe abstract: public sync_stream
{
public:
    virtual ~named_pipe() = default;
    virtual void connect(const char* pipe_name, const creation_option creation_opt) throw(...) = 0;
};

} // namespace interprocess
} // namespace voost
} // namespace vee

#endif // !_VEE_VOOST_PIPE_H_
