#ifndef _VEE_VOOST_XTIMER_H_
#define _VEE_VOOST_XTIMER_H_

#include <atomic>
#include <utility>
#include <vee/voost/asio.h>
#include <boost/asio/steady_timer.hpp>
#include <vee/voost/timer.h>
#include <vee/delegate.h>

namespace vee {
namespace voost {
namespace xtimer {

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
    xasync_timer(io_service_t& io_service = io_service_sigleton::get().io_service());
    virtual ~xasync_timer();
    virtual bool run(const unsigned int time_period_ms, const delegate_t& callback) override;
    virtual bool run(const second_type time_period, const delegate_t& callback) override;
    virtual bool run(const millisecond_type time_period, const delegate_t& callback) override;
    virtual bool stop() override;
    virtual bool is_running() const override;
protected:
    void _set_timer();
    void _on_timer(const boost::system::error_code& error, ::boost::asio::steady_timer* pTimer);
protected:
    io_service_t& _io_service;
    delegate_t    _e;
    timer_tick    _tick;
    ::boost::asio::steady_timer _timer;
    ::std::atomic<int> _running_state;
    ::std::chrono::milliseconds _period;
    enum class _state_id: int
    {
        IDLE = 0,
        RUN,
    };
};

} // namespace xtimer
} // namespace voost
} // namespace vee

#endif // _VEE_VOOST_XTIMER_H_