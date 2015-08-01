#include <vee/lock.h>

_VEE_BEGIN

spin_lock::spin_lock()
{
    _lock.clear();
}

_VEE_END