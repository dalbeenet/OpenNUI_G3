#ifndef _VEE_STRING_H_
#define _VEE_STRING_H_

#include <string>
#include <algorithm>

namespace vee {

using string = std::string;

inline string make_string(const char* str)
{
    string s(str);
    return s;
}

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

// templated version of my_equal so it could work with both char and wchar_t
template<typename charT>
struct my_equal
{
    my_equal(const std::locale& loc): loc_(loc)
    {
    }
    bool operator()(charT ch1, charT ch2)
    {
        return std::toupper(ch1, loc_) == std::toupper(ch2, loc_);
    }
private:
    const std::locale& loc_;
};


namespace strtool {

const int not_found = -1;

#pragma warning(disable:4996)
template <int BUFFER_SIZE = 512>
int find_case_insensitive(const char* src, const char* substr)
{
    char src_buffer[BUFFER_SIZE] = { 0 };
    char substr_buffer[BUFFER_SIZE] = { 0 };
    strcpy(src_buffer, src);
    strcpy(substr_buffer, substr);
    strupr(src_buffer);
    strupr(substr_buffer);
    char* ret = strstr(src_buffer, substr_buffer);
    if (ret == nullptr)
    {
        printf("src: %s\tsubstr: %s = not_found!\n", src_buffer, substr_buffer);
        return not_found;
    }
    else
    {
        return (int)(src_buffer) - (int)(ret);
    }
}
#pragma warning(default:4996)

// find substring (case insensitive)
template<typename T>
int ci_find_substr(const T& str1, const T& str2, const std::locale& loc = std::locale())
{
    typename T::const_iterator it = std::search(str1.begin(), str1.end(),
                                                str2.begin(), str2.end(), my_equal<typename T::value_type>(loc));
    if (it != str1.end()) return it - str1.begin();
    else return not_found; // not found
}

} // namespace strtool

}

#endif // _VEE_STRING_H_