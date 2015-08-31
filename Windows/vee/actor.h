#ifndef _VEE_ACTOR_H_
#define _VEE_ACTOR_H_

#include <vee/signal_channel.h>
#include <vee/delegate.h>
#include <vee/queue.h>
#include <tuple>
#include <mutex>

_VEE_BEGIN

namespace xactor {
    template <typename T>
    inline bool compare_and_swap_strong(::std::atomic<T>& dst, int exp, int value)
    {
        return dst.compare_exchange_strong(exp, value);
    }
}

template <class FTy>
class actor;

template <class RTy, class ...Args>
class actor < RTy(Args ...) >
{
    enum class sigid: int
    {
        spawn = 0,
        active,
        go_exit,
        exit_ok,
    };
public:
    using delegate_t = vee::delegate<RTy(Args ...)>;
    using argstuple_t = std::tuple<Args ...>;
    using mail_t = std::pair<delegate_t, argstuple_t>;

    actor(std::size_t mailbox_size):
        _queue_size(mailbox_size),
        _mailbox(mailbox_size),
        _mailbox_ptr(&_mailbox)
    {
        _spawn();
    }
    actor(vee::syncronized_ringqueue<mail_t>* mailbox_ptr):
        _queue_size(mailbox_ptr->capacity()),
        _mailbox(0),
        _mailbox_ptr(&mailbox_ptr)
    {
        
    }
    ~actor()
    {
        kill();
        printf("actor is destroyed!\n");
    }
    template <class Delegate, typename ...FwdArgs>
    inline int request(Delegate&& _delegate, FwdArgs&& ... args)
    {
        return this->request_with_argstuple(std::forward<Delegate>(_delegate), std::make_tuple(args ...));
    }
    template <class Delegate, class ArgsTuple>
    int request_with_argstuple(Delegate&& _delegate, ArgsTuple&& _tuple)
    {
        if (!_workable || _state.load() == DEAD)
        {
            return -1;
        }
        if (!(_mailbox_ptr->enqueue(std::make_pair(std::forward<Delegate>(_delegate), std::forward<ArgsTuple>(_tuple)))))
        {
            return -2;
        }
        int old_counter = _mailbox_counter++;
        if (old_counter == 0)
        {
            while (!xactor::compare_and_swap_strong(_state, IDLE, PROC))
            {
            //    throw std::runtime_error("actor::request CAS failure!!");
            }
            while (!_sigch.try_send(sigid::active))
            {
                //std::this_thread::sleep_for(std::chrono::microseconds::duration(1));
            }
        }
        return old_counter;
    }
    bool kill()
    {
        _workable = false;
        while (!xactor::compare_and_swap_strong(_state, IDLE, DEAD))
        {
            std::this_thread::sleep_for(std::chrono::microseconds::duration(1));
        }
        _thread.detach();
        while (!_sigch.try_send(sigid::go_exit))
        {
            //std::this_thread::sleep_for(std::chrono::microseconds::duration(1));
        }
        while (_sigch.recv() != sigid::exit_ok);
        return true;
    }
    bool respawn()
    {
        if (!xactor::compare_and_swap_strong(_state, DEAD, SPAWN))
            return false;
        _spawn();
        return true;
    }
    void waiting_for_exit()
    {
        if (false == _thread.joinable())
        {
            return;
        }
        else
        {
            _thread.join();
        }
    }
protected:
    void _spawn()
    {
        std::thread t(&actor<RTy(Args...)>::_actormain, this);
        if (_sigch.recv() != sigid::spawn)
        {
            throw std::runtime_error("actor constructor receives invalid signal!");
        }
        t.swap(_thread);
    }
    int _actormain()
    {
        if (_state != SPAWN)
        {
            throw std::runtime_error("actor thread creation failure! actor state has wrong value.");
        }
        _state.store(LOADED);
        while (!_sigch.try_send(sigid::spawn));
        _state.store(IDLE);
        _workable = true;
        while (true)
        {
            switch (_sigch.recv())
            {
            case sigid::active:
                _epoch();
                break;
            case sigid::go_exit:
                // exit process
                while(!_sigch.try_send(sigid::exit_ok));
                _state = DEAD;
                return 0;
            default:
                throw std::runtime_error("wrong signal received!");
                break;
            }
        }
        return 0;
    }
    bool _epoch()
    {
        mail_t current_job;
        while (true)
        {
            if (!(_mailbox_ptr->dequeue(current_job)))
            {
                return false;
            }
            delegate_t& e = current_job.first;
            argstuple_t& args = current_job.second;
            e(args);
            int old_counter = --_mailbox_counter;
            if (old_counter == 0)
                break;
        }
        if (!xactor::compare_and_swap_strong(_state, PROC, IDLE))
        {
            throw std::runtime_error("actor::_epoch CAS failure!!");
            return false;
        }
        return true;
    }
private:
    //! DISALLOW DEFAULT CONSTRUCTOR
    actor() = delete;
    //! DISALLOW COPY AND ASSIGNMENT
    actor(const actor < RTy(Args ...) >&) = delete;
    void operator=(const actor < RTy(Args ...) >&) = delete;
    //! DISALLOW MOVE AND ASSIGNMENT
    actor(actor < RTy(Args ...) >&&) = delete;
    void operator=(actor < RTy(Args ...) >&&) = delete;
private:
    vee::signal_channel<sigid> _sigch;
    vee::syncronized_ringqueue<mail_t> _mailbox;
    vee::syncronized_ringqueue<mail_t>* _mailbox_ptr;
    std::atomic<int> _mailbox_counter = 0;
    std::thread _thread;
    std::atomic<int> _state = SPAWN;
    std::size_t _queue_size;
    std::atomic<bool> _workable = false;
    static const int SPAWN  = 0;
    static const int LOADED = 1;
    static const int IDLE   = 3;
    static const int PROC   = 4;
    static const int DEAD   = 5;
};

template <class FTy>
class actor_group
{
    typedef std::shared_ptr< actor<FTy> > actor_ptr_type;
public:
    actor_group(unsigned int count, unsigned int mailbox_size)
    {
        for (unsigned int i = 0; i < count; ++i)
        {
            add_actor(mailbox_size);
        }
    }
    std::size_t add_actor(std::size_t mailbox_size)
    {
        std::lock_guard<std::mutex> guard(_container_lock);
        _actor_pointers.push_back(std::make_shared< actor<FTy> >(mailbox_size));
        int ret = ++_size;
        return ret;
    }
    std::size_t remove_actor()
    {
        std::lock_guard<std::mutex> guard(_container_lock);
        int ret = --_size;
        _actor_pointers.pop_back();
        return ret;
    }
    inline int capacity()
    {
        return _size;
    }
    inline actor<FTy>* at(unsigned int idx)
    {
        if (idx >= _actor_pointers.size())
        {
            return nullptr;
        }
        actor<FTy>* ptr = _actor_pointers[idx].get();
        return ptr;
    }
    inline actor<FTy>* operator[](unsigned int idx)
    {
        return this->at(idx);
    }
protected:
    std::vector<actor_ptr_type> _actor_pointers;
    std::atomic<int> _size;
    std::mutex  _container_lock;
};

_VEE_END

#endif