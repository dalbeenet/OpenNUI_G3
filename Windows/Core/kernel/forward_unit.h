#ifndef _OPENNUIG3_KERNEL_FORWARD_UNIT_H_
#define _OPENNUIG3_KERNEL_FORWARD_UNIT_H_
#include <kernel/device_manager.h>
#include <vee/voost/timer.h>

namespace kernel {

class forward_unit final
{
    DISALLOW_COPY_AND_ASSIGN(forward_unit);
    DISALLOW_MOVE_AND_ASSIGN(forward_unit);
public:
    using timer_t = ::vee::voost::timer::async_timer;
    using timer_ptr = ::std::shared_ptr<timer_t>;
    using timer_tick = timer_t::timer_tick;
    ~forward_unit();
    static ::std::shared_ptr<forward_unit> get_instance();
private:
    forward_unit();
    static void _on_timer(timer_tick tick);
    static void _poll_and_forward_once(device_manager::device_module_ptr module, 
                                       ::std::shared_ptr<unsigned char> color_frame_buffer,
                                       ::std::shared_ptr<unsigned char> depth_frame_buffer,
                                       ::std::shared_ptr<unsigned char> body_frame_buffer);
private:
    timer_ptr _timer;
    timer_t::delegate_t _callback;
};

} // namespace kernel

#endif // _OPENNUIG3_KERNEL_FORWARD_UNIT_H_