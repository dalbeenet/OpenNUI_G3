/******************************************************************* 
FileName: thread_model.h
Author  : vee
Date    : 2015-07-01 00:48:17
Desc    : thread model class
*******************************************************************/ 

#ifndef _VEE_LOCK_H_
#define _VEE_LOCK_H_

#include <atomic>
#include <mutex>
#include <vee/macro.h>

_VEE_BEGIN

struct empty_mutex
{
    __forceinline void lock() VEE_EMPTY_FUNCTION;
    __forceinline bool try_lock()
    {
        return true;
    }
    __forceinline void unlock() VEE_EMPTY_FUNCTION;
    __forceinline std::mutex::native_handle_type native_handle()
    {
        return NULL;
    }
};

class spin_lock
{
    DISALLOW_COPY_AND_ASSIGN(spin_lock);
public:
    spin_lock();
    inline void lock(std::memory_order order = std::memory_order_acquire)
    {
        while (_lock.test_and_set(order));
    }
    inline bool try_lock(std::memory_order order = std::memory_order_acquire)
    {
        return _lock.test_and_set(order);
    }
    inline void unlock(std::memory_order order = std::memory_order_release)
    {
        _lock.clear(order);
    }
protected:
    std::atomic_flag _lock;
};

_VEE_END

#endif // _VEE_LOCK_H_