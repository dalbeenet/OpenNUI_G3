/******************************************************************* 
FileName: delegate.h
Author  : vee
Date    : 2015-07-10 01:01:22
Desc    : c++ delegate class

*******************************************************************/ 

#ifndef _VEE_DELEGATE_H_
#define _VEE_DELEGATE_H_

#include <inttypes.h>
#include <cassert>
#include <map>
#include <vector>
#include <functional>
#include <vee/macro.h>
#include <vee/tuple_unpacker.h>

_VEE_BEGIN

template <class T>
void* pvoid_cast(T pointer)
{
    auto& ptr = pointer;
    void* addr = *(void**)(&ptr);
    return addr;
}

template <typename FuncSig, typename _Function>
bool compare_functions(const _Function& lhs, const _Function& rhs)
{
    typedef typename std::conditional
        <
        std::is_function<FuncSig>::value,
        typename std::add_pointer<FuncSig>::type,
        FuncSig
        > ::type target_type;
    if (const target_type* lhs_internal = lhs.template target<target_type>())
    {
        if (const target_type* rhs_internal = rhs.template target<target_type>())
            return *rhs_internal == *lhs_internal;
    }
    return false;
}

template <typename FTy>
class compareable_function: std::function < FTy >
{
    typedef std::function<FTy> _function_t;
    bool(*_type_holder)(const _function_t&, const _function_t&);
public:
    compareable_function() = default;
    template <typename CallableObj> compareable_function(CallableObj f):
        _function_t(f),
        _type_holder(compare_functions<CallableObj, _function_t>)
    {
        // empty
    }
    template <typename CallableObj> compareable_function& operator=(CallableObj f)
    {
        _function_t::operator =(f);
        _type_holder = compare_functions < CallableObj, _function_t > ;
        return *this;
    }
    friend bool operator==(const _function_t& lhs, const compareable_function& rhs)
    {
        return rhs._type_holder(lhs, rhs);
    }
    friend bool operator==(const compareable_function &lhs, const _function_t &rhs)
    {
        return rhs == lhs;
    }
};

template <class T>
class delegate
{
};

template <class RTy, class ...Args>
class delegate < RTy(Args ...) >
{
public:
    typedef delegate< RTy(Args ...) > this_t;
    typedef this_t& reference_t;
    typedef this_t&& rreference_t;
    typedef RTy(*funcptr_t)(Args ...);
    typedef int64_t key_t;
    typedef std::tuple<Args ...> argstuple_t;
    static const int number_of_args = std::tuple_size < argstuple_t >::value ;
private:
    typedef std::function< RTy(Args ...) > _binder_t;
    typedef compareable_function< RTy(Args ...) > _compareable_binder_t;
    typedef std::vector< _binder_t > _container_t;
    typedef std::map<key_t, _binder_t> _indexing_container_t;
public:
    // ctor
    delegate() = default;
    // dtor
    ~delegate() = default;
    // copy ctor
    delegate(const delegate < RTy(Args ...) >& rhs)
    {
        this->_deep_copy(rhs);
    }
    // move ctor
    delegate(delegate < RTy(Args ...) >&& rhs)
    {
        this->_deep_copy(static_cast<rreference_t>(rhs));
    }
    reference_t operator=(const delegate < RTy(Args ...) >& rhs)
    {
        this->_deep_copy(rhs);
        return *this;
    }
    reference_t operator=(delegate < RTy(Args ...) >&& rhs)
    {
        this->_deep_copy(static_cast<rreference_t>(rhs));
        return *this;
    }
    template <typename ...FwdArgs>
    void operator()(FwdArgs&& ...args)
    {
        for (auto& it : _container)
        {
            it.operator()(args...);
        }
        for (auto& it : _idx_container)
        {
            it.second.operator()(args...);
        }
    }
    template <class ArgsTuple>
    void operator()(ArgsTuple&& args)
    {
        for (auto& it : _container)
        {
            call_by_tuple(it, args);
        }
        for (auto& it : _idx_container)
        {
            call_by_tuple(it.second, args);
        }
    }
    template <class CallableObj>
    void operator+=(CallableObj func)
    {
        _binder_t binder(func);
        _container.push_back(std::move(binder));
    }
    template <class CallableObj>
    void operator+=(std::pair<key_t, CallableObj> key_func_pair)
    {
        _binder_t binder(key_func_pair.second);
        _idx_container.insert(std::make_pair(key_func_pair.first, binder));
    }
    template <class CallableObj>
    bool operator-=(CallableObj func)
    {
        _binder_t binder(func);
        for (auto iter = _container.begin(); iter != _container.end(); ++iter)
        {
            if (compare_functions<CallableObj>(*iter, binder))
            {
                _container.erase(iter);
                return true;
            }
        }
        printf("REMOVE FAILURE (function not found)\n");
        return false;
    }
    bool operator-=(_binder_t binder)
    {
        for (auto iter = _container.begin(); iter != _container.end(); ++iter)
        {
            if (compare_functions<RTy(Args ...)>(*iter, binder))
            {
                _container.erase(iter);
                return true;
            }
        }
        printf("REMOVE FAILURE (function not found)\n");
        return false;
    }
    bool operator-=(const key_t key)
    {
        for (auto iter = _idx_container.begin(); iter != _idx_container.end(); ++iter)
        {
            if (iter->first == key)
            {
                _idx_container.erase(iter);
                return true;
            }
        }
        printf("REMOVE FAILURE (key not found)\n");
        return false;
    }
private:
    void _deep_copy(const delegate < RTy(Args ...) >& rhs)
    {
        static int call_count = 0;
        //printf("LVALUE CALL %d\n", ++call_count);
        if (rhs._container.size() > 0)
            _container = rhs._container;
        if (rhs._idx_container.size() > 0)
            _idx_container = rhs._idx_container;
    }
    void _deep_copy(delegate < RTy(Args ...) >&& rhs)
    {
        static int call_count = 0;
        //printf("RVALUE CALL %d\n", ++call_count);
        if (rhs._container.size() > 0)
            _container = std::move(rhs._container);
        if (rhs._idx_container.size() > 0)
            _idx_container = std::move(rhs._idx_container);
    }
private:
    _container_t _container;
    _indexing_container_t _idx_container;
};

_VEE_END

#endif // _VEE_DELEGATE_H_