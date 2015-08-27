#ifndef _OPENNUIG3_KERNEL_XKERNEL_TASKAPI_H_
#define _OPENNUIG3_KERNEL_XKERNEL_TASKAPI_H_

#include <kernel/xkernel/parameters.h>

namespace xkernel {

enum class task_id
{
    test = 0,
};

namespace scheduler {

// Initialize scheduler
void initialize(unsigned int init_actors = parameters::INITIAL_ACTORS, unsigned int mailbox_size = parameters::ACTOR_MAILBOX_CAPACITY);

// Dispose scheduler
void dispose();

} // namespace scheduler

} // namespace xkernel

#endif // _OPENNUIG3_KERNEL_XKERNEL_TASKAPI_H_