/******************************************************************* 
FileName: flag.h
Author  : vee
Date    : 2015-07-01 00:42:48
Desc    : template flag class
*******************************************************************/ 

#ifndef _VEE_FLAG_H_
#define _VEE_FLAG_H_

#include <vee/macro.h>

_VEE_BEGIN

template <typename T>
class flag
{
public:
    typedef T       flag_type;
    typedef flag<T> this_type;
    flag(): _flag()
    {
        // Nothing to do.
    }
    ~flag();
    inline T get()
    {
        return _flag;
    }
    inline void set(T& val)
    {
        _flag = val;
    }
    inline bool is_true()
    {
        return static_cast<bool>(_flag);
    }
    inline bool is_false()
    {
        return _flag == 0;
    }
private:
    T _flag;
};

typedef flag<bool> bool_flag;
typedef flag<int>  integer_flag;

_VEE_END

#endif // _VEE_FLAG_H_