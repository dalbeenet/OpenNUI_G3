#include <vee/win32.h>
#include <vee/exception.h>
#include <vee/system.h>
#include <cstdio>
#pragma warning(disable:4996)
namespace vee {

win32_handle::~win32_handle() __noexcept
{
    close_nothrow();
}

win32_handle::win32_handle() __noexcept:
_handle(NULL)
{

}

win32_handle::win32_handle(win32_handle&& other) __noexcept:
_handle(other._handle)
{
    other.clear();
}

win32_handle::win32_handle(HANDLE&& handle) __noexcept:
_handle(handle)
{
    handle = NULL;
}

win32_handle& win32_handle::operator=(win32_handle&& other) __noexcept
{
    _handle = other._handle;
    other.clear();
    return *this;
}

win32_handle& win32_handle::operator=(HANDLE&& handle) __noexcept
{
    _handle = _handle;
    handle = NULL;
    return *this;
}

void win32_handle::close() throw(...)
{
    if (_handle != NULL)
    {
        if (!CloseHandle(_handle))
        {
            char buffer[256] = { 0, };
            sprintf(buffer, "%s> CloseHandle failure! GLE: %d", __FUNCSIG__, GetLastError());
            throw vee::exception(buffer, (int)system::error_code::win32_close_handle_failure);
        }
        _handle = NULL;
        return;
    }
}

void win32_handle::close_nothrow() __noexcept
{
    if (_handle != NULL)
    {
        if (!CloseHandle(_handle))
        {
            printf("%s> CloseHandle failure! GLE: %d\n", __FUNCSIG__, GetLastError());
            return;
        }
        _handle = NULL;
        return;
    }
}

} // namespcae vee
#pragma warning(default:4996)