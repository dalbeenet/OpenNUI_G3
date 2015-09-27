#include <kernel/session.h>
#include <atomic>

namespace kernel {

uint32_t session::make_unique_sid()
{
    static std::atomic_uint32_t counter(0);
    return ++counter;
}

} // namespace kernel