#include <vee/voost/string.h>
#include <boost/uuid/sha1.hpp>
#include <iostream>

namespace vee {
namespace voost {

::std::vector<unsigned char> sha1_hashing(const string& dst)
{
    ::boost::uuids::detail::sha1 sha1;
    ::std::vector<unsigned char> hash(20, 0);
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

void print_hash_code(::std::vector<unsigned char>& dst)
{
        ::std::cout << "SHA1: " << std::hex;
        for (int i = 0; i < 20; ++i)
        {
            ::std::cout << ((dst[i] & 0x000000F0) >> 4)
                << (dst[i] & 0x0000000F);
        }
        ::std::cout << ::std::endl; // Das wars  
}

} // namespace voost
} // namespace vee