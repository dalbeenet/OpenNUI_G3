#include <vee/voost/net.h>

namespace vee {
namespace voost {
namespace net {

namespace websocket {

net::size_t ws_stream::write(void* data, net::size_t len) throw(...)
{
    io_result result = write(opcode_id::binary_frame, data, len);
    return result.header_size + result.payload_size;
}

net::size_t ws_stream::read(void* buffer, net::size_t buf_capacity) throw(...)
{
    return read_payload_only(buffer, buf_capacity).payload_size;
}

} // namespace websocket

} // namespace net
} // namespace voost
} // namespace vee