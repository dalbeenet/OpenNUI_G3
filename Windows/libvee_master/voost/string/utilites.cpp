#include <vee/voost/string.h>
#include <iostream>

namespace vee {
namespace voost {

void print_hash_code(const char* dst)
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