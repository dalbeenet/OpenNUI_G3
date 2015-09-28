#include <kernel/scheduler.h>

namespace kernel {

::vee::scheduler<void()> scheduler(4, 1000);

} // namespace kernel