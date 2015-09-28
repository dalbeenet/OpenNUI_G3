#ifndef _OPENNUIG3_KERNEL_GATEWAY_H_
#define _OPENNUIG3_KERNEL_GATEWAY_H_

#include <vee/voost/net.h>

namespace kernel {

class gateway final
{
    DISALLOW_COPY_AND_ASSIGN(gateway);
    DISALLOW_MOVE_AND_ASSIGN(gateway);
public:
    using net_stream = ::vee::voost::net::net_stream;
    using net_server = ::vee::voost::net::net_server;
    using net_stream_ptr = ::std::shared_ptr<net_stream>;
    using net_server_ptr = ::std::shared_ptr<net_server>;
    using byte = ::vee::voost::net::byte;
    using net_error_code = vee::voost::net::error_code;
    using operation_result = ::vee::system::operation_result;
    static ::std::shared_ptr<gateway> get_instance();
    ~gateway();

private:
    gateway();
    void _accept_handler(operation_result& result, net_stream_ptr stream);
    static void _handshake(net_stream_ptr raw_stream);
// Private member variables
private:
    net_server_ptr _server;
};

} // namespace kernel

#endif // !_OPENNUIG3_KERNEL_GATEWAY_H_
