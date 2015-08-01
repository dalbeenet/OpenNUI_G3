#ifndef _VEE_QUEUE_H_
#define _VEE_QUEUE_H_

#include <atomic>
#include <vector>
#include <vee/lock.h>

_VEE_BEGIN

template <
    class DataType, 
    class ItemGuardType = vee::spin_lock, 
    class IndexGuardType = vee::spin_lock >
class syncronized_ringqueue
{
public:
    typedef syncronized_ringqueue<DataType, ItemGuardType, IndexGuardType> this_type;
    typedef DataType data_type;
    typedef ItemGuardType item_guard_type;
    typedef IndexGuardType index_guard_type;
    syncronized_ringqueue(const std::size_t capacity):
        _capacity(capacity),
        _item_guards(capacity),
        _item_container(capacity),
        _overwrite_mode(false),
        _rear(0),
        _front(0),
        _size(0)
    {
        // nothing to do.
    }
    syncronized_ringqueue(const this_type& other): syncronized_ringqueue(other._capacity)
    {
        this->operator=(const_cast<this_type&>(other));
    }
    syncronized_ringqueue(this_type&& other): syncronized_ringqueue(other._capacity)
    {
        this->operator=(static_cast<this_type&&>(other));
    }
    ~syncronized_ringqueue()
    {
        // nothing to do.
    }
    inline bool is_empty()
    {
        if (_size == 0)
            return true;
        return false;
    }
    inline bool is_full()
    {
        if (_size == _capacity)
            return true;
        return false;
    }
    template <class FwdDataTy>
    bool enqueue(FwdDataTy&& data)
    {
        int rear = 0;
        std::unique_lock<typename item_guard_type> item_lock;
        {
            std::lock_guard<typename index_guard_type> index_lock(_index_guard);
            rear = _rear;
            if (is_full())
            {
                if (!_overwrite_mode)
                {
                    return false;
                }
                ++_rear %= _capacity;
                ++_front %= _capacity;
            }
            else
            {
                ++_rear %= _capacity;
                ++_size;
            }
            std::unique_lock<typename item_guard_type> item_lock_temp(_item_guards[rear], std::adopt_lock);
            std::swap(item_lock, item_lock_temp);
        }
        _item_container[rear] = std::forward<FwdDataTy>(data);
        return true;
    }
    bool shallow_dequeue()
    {
        std::lock_guard<typename index_guard_type> index_lock(_index_guard);
        if (!_size)
        {
            return false;
        }
        --_size;
        int front = _front;
        ++_front %= _capacity;
        return true;
    }
    bool dequeue()
    {
        data_type temp;
        return this->dequeue(temp);
    }
    bool dequeue(data_type& out)
    {
        int front = 0;
        std::unique_lock<typename item_guard_type> item_lock;
        {
            std::lock_guard<typename index_guard_type> index_lock(_index_guard);
            if (!_size)
            {
                return false;
            }
            --_size;
            front = _front;
            ++_front %= _capacity;

            std::unique_lock<typename item_guard_type> item_lock_temp(_item_guards[front], std::adopt_lock);
            std::swap(item_lock, item_lock_temp);
        }
        typedef std::remove_reference<data_type>::type orig_type;
        typedef std::conditional <
            std::is_move_assignable<orig_type>::value,
            std::add_rvalue_reference<orig_type>::type,
            std::add_lvalue_reference<orig_type>::type >::type request_type;
        out = static_cast<request_type>(_item_container[front]);
        return true;
    }
    void clear()
    {
        while (this->dequeue());
    }
    inline std::size_t capacity()
    {
        return _capacity;
    }
    /*bool operator=(const this_type& other)
    {
    return this->copy_from(const_cast<this_type&>(other));
    }*/
    template <class SyncronizedQueue>
    bool operator=(typename SyncronizedQueue&& rhs)
    {
        typedef std::remove_reference<SyncronizedQueue>::type rhs_type;
        this_type& lhs = *this;
        lhs.clear();
        if (lhs._capacity < rhs._capacity)
        {
            return false;
        }
        std::lock_guard<typename rhs_type::index_guard_type> rhs_index_guard(rhs._index_guard);
        typedef std::conditional <
            std::is_rvalue_reference<SyncronizedQueue>::value &&
            std::is_move_assignable<typename rhs_type::data_type>::value,
            std::add_rvalue_reference<typename rhs_type::data_type>::type,
            std::add_lvalue_reference<typename rhs_type::data_type>::type > ::type request_type;
        int rear = rhs._rear;
        int size = rhs._size;
        for (int i = 0; i < size; ++i)
        {
            std::lock_guard<typename rhs_type::item_guard_type> item_guard(rhs._item_guards[rear]);
            lhs.enqueue(static_cast<request_type>(rhs._item_container[rear]));
            ++rear %= rhs._capacity;
        }
        return true;
    }
private:
    // default ctor is deleted.
    syncronized_ringqueue() = delete;
protected:
    std::vector< typename item_guard_type > _item_guards;
    std::vector<data_type>  _item_container;
    std::size_t _capacity;
    typename index_guard_type _index_guard;
    bool _overwrite_mode;
    int _rear;
    int _front;
    int _size;
};

template <class DataType>
class ringqueue: public syncronized_ringqueue < DataType, vee::empty_mutex, vee::empty_mutex >
{
public:
    typedef syncronized_ringqueue<DataType, vee::empty_mutex, vee::empty_mutex> base_type;
    typedef ringqueue<DataType> this_type;
};

_VEE_END

#endif // _VEE_QUEUE_H_