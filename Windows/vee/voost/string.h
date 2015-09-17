#ifndef _VEE_VOOST_STRING_H_
#define _VEE_VOOST_STRING_H_

#include <array>
#include <vector>
#include <vee/string.h>

namespace vee {
namespace voost {

::std::vector<unsigned char> sha1_hashing(const string& dst);
void print_hash_code(::std::vector<unsigned char>& dst);

namespace base64 {

string decode(const ::std::vector<unsigned char> &dst);
string encode(const ::std::vector<unsigned char> &dst); 
string encode(const std::vector<unsigned char>& data);

} // namespace base64

} // namespace voost
} // namespace vee

#endif // _VEE_VOOST_STRING_H_