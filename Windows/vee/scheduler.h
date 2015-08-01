#ifndef _VEE_SCHEDULER_H_
#define _VEE_SCHEDULER_H_

#include <mutex>
#include <vee/actor.h>

_VEE_BEGIN

template <class FTy>
class scheduler
{
public:
    scheduler(unsigned int count, unsigned int mailbox_size):
        actor_group(count, mailbox_size)
    {
        
    }
    template <class Delegate, typename ...FwdArgs>
    inline int request(Delegate&& _delegate, FwdArgs&& ... args)
    {
        return this->request(std::forward<Delegate>(_delegate), std::make_tuple(args ...));
    }
    template <class Delegate, class ArgsTuple>
    bool request(Delegate&& _delegate, ArgsTuple&& _tuple)
    {
        actor<FTy>* current_actor = nullptr;
        while (true)
        {
            int i = _get_current_actor_index();
            if (i < 0)
                return false;
            current_actor = actor_group.at(i);
            if (current_actor == nullptr)
            {
                continue;
            }
            if (current_actor->request(
                std::forward<Delegate>(_delegate),
                std::forward<ArgsTuple>(_tuple)) >= 0)
            {
                break;
            }
        }
        return true;
    }
private:
    int _get_current_actor_index()
    {
        static std::atomic<int> counter = 0;
        int i = counter++;
        int capacity = actor_group.capacity();
        if (capacity == 0)
            return -1;
        i %= capacity;
        return i;
    }
public:
    actor_group<FTy> actor_group;
};

_VEE_END

#endif // _VEE_SCHEDULER_H_