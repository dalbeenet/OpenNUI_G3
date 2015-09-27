#ifndef _OPENNUIG3_KERNEL_XKERNEL_TASKAPI_H_
#define _OPENNUIG3_KERNEL_XKERNEL_TASKAPI_H_

#include <kernel/xkernel/parameters.h>
#include <vee/delegate.h>

namespace xkernel {

//using task_sig = void(void*);
//using task_t = ::vee::delegate<task_sig>;
//
//enum class task_id
//{
//    test = 0,
//};
//
//namespace scheduler {
//
//// Initialize scheduler
//void initialize(unsigned int init_actors = parameters::INITIAL_ACTORS, unsigned int mailbox_size = parameters::ACTOR_MAILBOX_CAPACITY);
//
//// Dispose scheduler
//void dispose();
//
//// Request task to scheduler (Reference delegate)
//void request(task_t& task, void* arg);
//// Request task to scheduler (RReference delegate)
//void request(task_t&& task, void* arg);
//// Request task to scheduler (Function type)
//void request(std::function<task_sig> function, void* arg);

} // namespace scheduler

} // namespace xkernel

#endif // _OPENNUIG3_KERNEL_XKERNEL_TASKAPI_H_