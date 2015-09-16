#ifndef _VEE_VOOST_STRING_H_
#define _VEE_VOOST_STRING_H_

#include <array>
#include <vee/string.h>

namespace vee {
namespace voost {

string sha1_hashing(const string& dst);
void print_hash_code(const char* dst);

namespace base64 {

string decode(const string &dst);
string encode(const string &dst);

} // namespace base64

} // namespace voost
} // namespace vee

#endif // _VEE_VOOST_STRING_H_