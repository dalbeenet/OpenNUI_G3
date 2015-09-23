#ifndef _VEE_SYSTEM_H_
#define _VEE_SYSTEM_H_

#include <vee/string.h>

namespace vee {
namespace system {

enum class error_code: unsigned int
{
    success = 0,
    stream_connection_failure,
    server_accept_failure,
    stream_send_failure,
    stream_read_failure,
    stream_disconnected_by_host,
    stream_closed,
    rfc6455_handshake_failure,
    rfc6455_websocket_heartbeat,
};

struct operation_result
{
    error_code error;
    string     desc;
};

} // namespace system
} // namespace vee

#endif // !_VEE_SYSTEM_H_
