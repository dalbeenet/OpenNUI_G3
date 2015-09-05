#ifndef _VEE_VOOST_XTIMER_H_
#define _VEE_VOOST_XTIMER_H_

#include <atomic>
#include <thread>
#include <utility>
#include <boost/asio.hpp>
#include <boost/asio/steady_timer.hpp>
#include <vee/voost/timer.h>
#include <vee/delegate.h>
#include <vee/signal_channel.h>

namespace vee {
namespace voost {
namespace xtimer {

class io_service_wrapper
{
    DISALLOW_COPY_AND_ASSIGN(io_service_wrapper);
    DISALLOW_MOVE_AND_ASSIGN(io_service_wrapper);
public:
    using io_service_t = ::boost::asio::io_service;
    using worker_t = boost::asio::io_service::work;
    io_service_wrapper();
    ~io_service_wrapper();
    io_service_t io_service;
    worker_t worker;
private:
    ::std::thread _thread;
    alram_channel _sigch;
};

io_service_wrapper::io_service_wrapper():
worker(this->io_service),
_thread([this]()
{
    //! io_service.run()이 실패했을 경우는 고려되지 않음
    io_service.run();
    while (!_sigch.try_send());
})
{

}

io_service_wrapper::~io_service_wrapper()
{
    io_service.stop();
    _thread.detach();
    _sigch.recv();
}

extern io_service_wrapper g_wrapper;

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
    xasync_timer(io_service_wrapper& io_service_wrapper = g_wrapper);
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