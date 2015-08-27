#include <kernel/kernel.h>
#include <vee/scheduler.h>

using namespace std;

namespace xkernel { 
namespace scheduler {

using scheduler_t = vee::scheduler<void(task_id, void* arg)>;
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

} // namespace scheduler 
} // namespace xkernel