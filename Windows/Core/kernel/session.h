#ifndef _OPENNUIG3_KERNEL_SESSION_H_
#define _OPENNUIG3_KERNEL_SESSION_H_
#include <vee/stream.h>
#include <kernel/protocol.h>

namespace kernel {

class session abstract
{
public:
    using  session_id_t = uint32_t;
    static session_id_t make_unique_sid();
    virtual ~session() = default;
    virtual session_id_t get_id() = 0;
};
using session_ptr = ::std::shared_ptr<session>;

} // namespace kernel

#endif // _OPENNUIG3_KERNEL_SESSION_H_