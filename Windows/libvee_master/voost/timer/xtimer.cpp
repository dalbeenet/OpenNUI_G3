#include <vee/voost/xtimer.h>

namespace vee {
namespace voost {
namespace xtimer {

::boost::asio::io_service timer_io_service;

xasync_timer::xasync_timer(io_service_t& io_service):
_io_service(io_service)
{

}

} // namespace xtimer
} // namespace voost
} // namespace vee