#ifndef _VEE_EXCEPTION_H_
#define _VEE_EXCEPTION_H_

#include <string>

namespace vee {

#if (_MSC_VER == 1900) // VS2015
#define __noexcept noexcept
#else
#define __noexcept throw()
#endif

class exception
{
public:
    using string = ::std::string;
    static const int desc_length = 1024;
    exception(const char* _desc, int code);
    exception(string& _desc, int _code);
    exception(string&& _desc, int _code);
    inline const char* what() const
    {
        return desc.data();
    }
    string desc;
    int code;
};

} // namespace vee

#endif // _VEE_EXCEPTION_H_