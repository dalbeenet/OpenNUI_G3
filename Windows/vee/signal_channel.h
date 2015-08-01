#ifndef _VEE_SIGNAL_H_ 
#define _VEE_SIGNAL_H_

#include <vee/macro.h>
#include <exception>
#include <atomic>
#include <condition_variable>
#include <future>

_VEE_BEGIN

template <class T>
class signal_channel
{
    DISALLOW_COPY_AND_ASSIGN(signal_channel<T>);
    DISALLOW_MOVE_AND_ASSIGN(signal_channel<T>);
public:
    typedef signal_channel<T> this_t;
    typedef this_t& reference_t;
    typedef this_t&& rreference_t;
    // ctor
    signal_channel()
    {
        // empty
    }
    // dtor
    ~signal_channel()
    {
        // empty
    }
    T recv()
    {
        if (_state == WAIT)
        {
            throw std::runtime_error("This signal object is already on waiting state!");
        }
        while (!_compare_and_swap_weak(_state, IDLE, WAIT));
        std::promise<T> receiver;
        std::future<T> future = receiver.get_future();
        _receiver_ptr = &receiver;
        T result = future.get();
        _receiver_ptr = nullptr;
        while (!_compare_and_swap_weak(_state, SIG, IDLE));
        return result;
    }
    bool try_send(T value)
    {
        if (!_compare_and_swap_weak(_state, WAIT, SIG))
        {
            return false;
        }
        while (_receiver_ptr == nullptr);
        _receiver_ptr->set_value(value);
        return true;
    }
protected:
    inline bool _compare_and_swap_weak(std::atomic<int>& dst, int exp, int value)
    {
        return dst.compare_exchange_weak(exp, value);
    }

protected:
    std::promise<T>* volatile _receiver_ptr = nullptr;
    std::atomic<int> _state = IDLE;
    static const int IDLE = 0;
    static const int WAIT = 1;
    static const int SIG  = 2;
};

template <>
class signal_channel<void>
{
    DISALLOW_COPY_AND_ASSIGN(signal_channel);
    DISALLOW_MOVE_AND_ASSIGN(signal_channel);
public:
    typedef signal_channel<void> this_t;
    typedef this_t& reference_t;
    typedef this_t&& rreference_t;
    // ctor
    signal_channel()
    {
        // empty
    }
    // dtor
    ~signal_channel()
    {
        // empty
    }
    void recv()
    {
        if (_state == WAIT)
        {
            throw std::runtime_error("This signal object is already on waiting state!");
        }
        while (!_compare_and_swap_weak(_state, IDLE, WAIT));
        std::promise<int> receiver;
        std::future<int> future = receiver.get_future();
        _receiver_ptr = &receiver;
        auto result = future.get();
        _receiver_ptr = nullptr;
        while (!_compare_and_swap_weak(_state, SIG, IDLE));
    }
    bool try_send()
    {
        if (!_compare_and_swap_weak(_state, WAIT, SIG))
        {
            return false;
        }
        while (_receiver_ptr == nullptr);
        _receiver_ptr->set_value(1);
        return true;
    }
protected:
    inline bool _compare_and_swap_weak(std::atomic<int>& dst, int exp, int value)
    {
        return dst.compare_exchange_weak(exp, value);
    }

protected:
    std::promise<int>* _receiver_ptr = nullptr;
    std::atomic<int> _state = IDLE;
    static const int IDLE = 0;
    static const int WAIT = 1;
    static const int SIG = 2;
};

typedef signal_channel<void> alram_channel;

_VEE_END

#endif // _VEE_SIGNAL_H_