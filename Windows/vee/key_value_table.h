#ifndef _VEE_KEY_VALUE_TABLE_H_
#define _VEE_KEY_VALUE_TABLE_H_

#include <utility>
#include <vector>
#include <map>
#include <vee/lock.h>

namespace vee {

template < class _Kty, // key type
class _Ty, // mapped type
class MutexTy = empty_mutex >
class key_value_table
{
public:
    typedef key_value_table<_Kty, _Ty> this_type;
    typedef std::map<_Kty, _Ty> container_type;
    typedef MutexTy  mutex_type;
    typedef _Kty    key_type;
    typedef _Ty     value_type;
public:
    key_value_table(): _c(), _m()
    {
        // nothing to do.
    }
    key_value_table(const key_value_table& other): key_value_table()
    {
        ::std::lock_guard<MutexTy> g(other._m);
        _c = other._c;
    }
    key_value_table(key_value_table&& other)
    {
        ::std::lock_guard<MutexTy> g(_m);
        _c.swap(std::move(other._c));
    }
    ~key_value_table() = default;
    key_value_table& operator=(const key_value_table& other)
    {
        ::std::lock_guard<MutexTy> g(_m);
        ::std::lock_guard<MutexTy> g(other._m);
        _c = other._c;
        return *this;
    }
    key_value_table& operator=(const key_value_table&& other)
    {
        ::std::lock_guard<MutexTy> g(_m);
        ::std::lock_guard<MutexTy> g(other._m);
        _c.swap(std::move(other._c));
        return *this;
    }
    bool insert(const _Kty& key, const _Ty& value)
    {
        ::std::lock_guard<MutexTy> g(_m);
        auto result = _c.insert(container_type::value_type(key, value));
        if (false == result.second)
        {
            return false;
        }
        return true;
    }
    bool remove(const _Kty& key)
    {
        ::std::lock_guard<MutexTy> g(_m);
        auto nDeleted = _c.erase(key);
        if (nDeleted > 0)
        {
            return true;
        }
        return false;
    }
    bool get_value(const _Kty& key, _Ty& dst)
    {
        if (!contains(key))
        {
            return false;
        }
        ::std::lock_guard<MutexTy> _g(_m);

        dst = _c.at(key);
        return true;
    }
    _Ty* get_pointer(const _Kty& key)
    {
        if (!contains(key))
        {
            return nullptr;
        }
        ::std::lock_guard<MutexTy> _g(_m);

        return &_c.at(key);
    }
    bool contains(const _Kty& key)
    {
        ::std::lock_guard<MutexTy> _g(_m);
        auto ret = _c.find(key);
        if (ret == _c.end()) return false;
        return true;
    }
    ::std::vector<_Kty> get_all_keys()
    {
        ::std::vector<_Kty> result;
        ::std::lock_guard<MutexTy> _g(_m);
        for (auto& it : _c)
        {
            result.push_back(it.first);
        }
        return result;
    }
    ::std::vector<_Ty> get_all_value_copies()
    {
        ::std::vector<_Ty> result;
        ::std::lock_guard<MutexTy> _g(_m);
        for (auto& it : _c)
        {
            result.push_back(it.second);
        }
        return result;
    }
    inline container_type& container()
    {
        return _c;
    }
    inline mutex_type& mutex()
    {
        return _m;
    }
private:
    container_type _c;
    mutable mutex_type _m;
};

} // namespace vee

#endif // _VEE_KEY_VALUE_TABLE_H_