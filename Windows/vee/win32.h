#ifndef _VEE_WIN32_H_
#define _VEE_WIN32_H_

#include <vee/exception.h>
#include <vee/macro.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#undef WIN32_LEAN_AND_MEAN

namespace vee {

class win32_handle
{
    DISALLOW_COPY_AND_ASSIGN(win32_handle);
public:
    ~win32_handle() __noexcept;
    win32_handle()  __noexcept;
    win32_handle(win32_handle&&) __noexcept;
    win32_handle(HANDLE&&) __noexcept;
    win32_handle& operator=(win32_handle&&)__noexcept;
    win32_handle& operator=(HANDLE&&)__noexcept;
    void close() throw(...);
    void close_nothrow() __noexcept;
    inline HANDLE& operator()()
    {
        return _handle;
    };
    inline HANDLE& get()
    {
        return _handle;
    }
    inline win32_handle& operator=(HANDLE& handle)
    {
        _handle = handle;
        return *this;
    }
    inline win32_handle& set(HANDLE& handle)
    {
        _handle = handle;
        return *this;
    }
    inline void clear()
    {
        _handle = NULL;
    }
    inline bool compare(const HANDLE& handle) const
    {
        return _handle == handle;
    }
    inline bool compare(HANDLE&& handle) const
    {
        return _handle == handle;
    }
    inline bool compare(const win32_handle& other) const
    {
        return _handle == other._handle;
    }
    inline bool compare(win32_handle&& other) const
    {
        return _handle == other._handle;
    }
    inline bool operator==(const HANDLE& handle) const
    {
        return _handle == handle;
    }
    inline bool operator==(HANDLE&& handle) const
    {
        return _handle == handle;
    }
    inline bool operator==(const win32_handle& other) const
    {
        return _handle == other._handle;
    }
    inline bool operator==(win32_handle&& other) const
    {
        return _handle == other._handle;
    }
protected:
    HANDLE _handle;
};

} // namespace vee

#endif // !_VEE_WIN32_H_