#include <vee/voost/net.h>

namespace vee {
namespace voost {
namespace net {

namespace websocket {

net::size_t stream_interface::write(void* data, net::size_t len) throw(...)
{
    return write_binary(data, len);
}

net::size_t stream_interface::read(void* buffer, net::size_t buf_capacity) throw(...)
{
    return read_payload_only(buffer, buf_capacity);
}

} // namespace websocket

} // namespace net
} // namespace voost
} // namespace vee