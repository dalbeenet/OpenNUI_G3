#include <kernel/kernel.h>
#include <vee/scheduler.h>

using namespace std;

namespace xkernel { 
namespace scheduler {

using scheduler_t = vee::scheduler<task_sig>;
unique_ptr<scheduler_t> scheduler_ptr(nullptr);

void initialize(unsigned int init_actors, unsigned int mailbox_size)
{
    if (scheduler_ptr == nullptr)
    {
        scheduler_ptr = make_unique<scheduler_t>(init_actors, mailbox_size);
    }
}

void dispose()
{
    if (scheduler_ptr != nullptr)
    {
        scheduler_ptr.release();
        scheduler_ptr = nullptr;
    }
}

void request(task_t& task, void* arg)
{
    scheduler_ptr->request(task, arg);
}

void request(task_t&& task, void* arg)
{
    scheduler_ptr->request(::std::move(task), arg);
}

void request(std::function<task_sig> function, void* arg)
{
    scheduler_ptr->request(vee::make_delegate<task_sig>(function), arg);
}

} // namespace scheduler 
} // namespace xkernel