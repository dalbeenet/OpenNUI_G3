#ifndef _VEE_STRING_H_
#define _VEE_STRING_H_

#include <string>

namespace vee {

using string = std::string;

inline string trim_left(const string& str)
{
    string::size_type n = str.find_first_not_of(" \t\v\n");
    return n == string::npos ? str : str.substr(n, str.length());
}

inline string trim_right(const string& str)
{
    string::size_type n = str.find_last_not_of(" \t\v\n");
    return n == string::npos ? str : str.substr(0, n + 1);
}

inline string trim(const string& str)
{
    return trim_left(trim_right(str));
}

}

#endif // _VEE_STRING_H_