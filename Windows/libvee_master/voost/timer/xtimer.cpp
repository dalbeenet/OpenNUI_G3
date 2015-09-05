#include <utility>
#include <vee/voost/xtimer.h>

namespace /*unnamed*/ {
template <typename T>
inline bool compare_and_swap_strong(::std::atomic<T>& dst, int exp, int value)
{
    return dst.compare_exchange_strong(exp, value);
}
}

namespace vee {
namespace voost {

namespace timer {

std::shared_ptr<async_timer> create_timer()
{
    return std::make_shared<xtimer::xasync_timer>();
}

}

namespace xtimer {

xasync_timer::xasync_timer(io_service_t& io_service):
_io_service(io_service),
_timer(io_service)
{

}

xasync_timer::~xasync_timer()
{
    stop();
}

bool xasync_timer::run(const unsigned int time_period_ms, const delegate_t& callback)
{
    if (compare_and_swap_strong(_running_state, (int)_state_id::IDLE, (int)_state_id::RUN) == false)
        return false;
    _e = callback;
    _tick = 0;
    _running_state = true;
    _period = ::std::chrono::milliseconds::duration(time_period_ms);
    _set_timer();
    return true;
}

bool xasync_timer::run(const second_type time_period, const delegate_t& callback)
{
    if (compare_and_swap_strong(_running_state, (int)_state_id::IDLE, (int)_state_id::RUN) == false)
        return false;
    _e = callback;
    _tick = 0;
    _running_state = true;
    _period = ::std::chrono::milliseconds::duration(time_period.value * 1000);
    _set_timer();
    return true;
}

bool xasync_timer::run(const millisecond_type time_period, const delegate_t& callback)
{
    if (compare_and_swap_strong(_running_state, (int)_state_id::IDLE, (int)_state_id::RUN) == false)
        return false;
    _e = callback;
    _tick = 0;
    _running_state = true;
    _period = ::std::chrono::milliseconds::duration(time_period.value);
    _set_timer();
    return true;
}

bool xasync_timer::stop()
{
    if (compare_and_swap_strong(_running_state, (int)_state_id::RUN, (int)_state_id::IDLE) == false)
        return false;
    //_stop_alram_channel.recv();
    return true;
}

bool xasync_timer::is_running() const
{
    if (_running_state.load() == (int)_state_id::RUN)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void xasync_timer::_set_timer()
{
    _timer.expires_from_now(_period);
    _timer.async_wait(
        std::bind(&xasync_timer::_on_timer,
        this,
        std::placeholders::_1,
        &_timer)
        );
}

void xasync_timer::_on_timer(const boost::system::error_code& error, ::boost::asio::steady_timer* pTimer)
{
    if (!error)
    {
        if (is_running() == (int)_state_id::RUN)
        {
            ++_tick;
            _e(_tick);
            _set_timer();
        }
        else
        {
            //while (!_stop_alram_channel.try_send());
        }
    }
    else
    {
        printf("timer> error no: %d, message: %s\n", error.value(), error.message().data());
    }
}

} // namespace xtimer
} // namespace voost
} // namespace vee