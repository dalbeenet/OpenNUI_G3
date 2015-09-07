#include <vee/voost/ws_stream.h>

namespace vee {
namespace voost {
namespace net {
namespace ws {

namespace /* unnamed */ {
inline ::boost::asio::ip::address string_to_ipaddr(const char* str)
{
    return ::boost::asio::ip::address::from_string(str);
}
} // unnamed namespace

} // namespace ws
} // namespace net
} // namespace voost
} // namespace vee