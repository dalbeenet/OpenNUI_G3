#ifndef _VEE_VOOST_STRING_H_
#define _VEE_VOOST_STRING_H_

#include <array>
#include <vee/string.h>
#include <boost/uuid/sha1.hpp>

namespace vee {
namespace voost {

template <int BUFFER_MAX = 1024>
::std::array<char, BUFFER_MAX> sha1_hashing(const string& dst)
{
    boost::uuids::detail::sha1 sha1;
    ::std::array<char, 1024> hash;
    sha1.process_bytes(dst.c_str(), dst.size());
    unsigned int digest[5];
    sha1.get_digest(digest);
    for (int i = 0; i < 5; ++i)
    {
        const char* tmp = reinterpret_cast<char*>(digest);
        hash[i * 4] = tmp[i * 4 + 3];
        hash[i * 4 + 1] = tmp[i * 4 + 2];
        hash[i * 4 + 2] = tmp[i * 4 + 1];
        hash[i * 4 + 3] = tmp[i * 4];
    }
    return hash;
}

void print_hash_code(const char* dst);

} // namespace voost
} // namespace vee

#endif // _VEE_VOOST_STRING_H_