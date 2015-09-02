#ifndef _VEE_VOOST_XTIMER_H_
#define _VEE_VOOST_XTIMER_H_

#include <boost/asio.hpp>
#include <vee/voost/timer.h>
#include <vee/delegate.h>

namespace vee {
namespace voost {
namespace xtimer {

extern ::boost::asio::io_service timer_io_service;

class xasync_timer: public timer::async_timer
{
    DISALLOW_COPY_AND_ASSIGN(xasync_timer);
    DISALLOW_MOVE_AND_ASSIGN(xasync_timer);
public:
    using io_service_t = ::boost::asio::io_service;
    using second_type = timer::second_type;
    using millisecond_type = timer::millisecond_type;
    using timer_tick = async_timer::timer_tick;
    using delegate_t = async_timer::delegate_t;
    xasync_timer(io_service_t& io_service);
    virtual ~xasync_timer();
    virtual bool run(const millisecond_type time_period, const delegate_t& callback) override;
    virtual bool run(const second_type time_period, const delegate_t& callback) override;
    virtual bool stop() override;
protected:
    io_service_t& _io_service;
};

} // namespace xtimer
} // namespace voost
} // namespace vee

#endif // _VEE_VOOST_XTIMER_H_