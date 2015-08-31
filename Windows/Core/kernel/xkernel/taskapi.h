#ifndef _OPENNUIG3_KERNEL_XKERNEL_TASKAPI_H_
#define _OPENNUIG3_KERNEL_XKERNEL_TASKAPI_H_

#include <kernel/xkernel/parameters.h>
#include <vee/delegate.h>
#include <cstdint>

namespace xkernel {

using task_t = ::vee::delegate<void(::std::int64_t*)>;

enum class task_id
{
    test = 0,
};

namespace scheduler {

// Initialize scheduler
void initialize(unsigned int init_actors = parameters::INITIAL_ACTORS, unsigned int mailbox_size = parameters::ACTOR_MAILBOX_CAPACITY);

// Dispose scheduler
void dispose();

// Request task to scheduler (Reference delegate)
void request(task_t& task, ::std::int64_t* arg);
// Request task to scheduler (RReference delegate)
void request(task_t&& task, ::std::int64_t* arg);

} // namespace scheduler

} // namespace xkernel

#endif // _OPENNUIG3_KERNEL_XKERNEL_TASKAPI_H_