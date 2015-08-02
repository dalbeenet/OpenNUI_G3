#include <opennui/opennui.h>
#include <atomic>

//TODO: DEF FILE

class test_device: public _OPENNUI opennui_device
{
    DEFINE_OPENNUI_DEVICE(test_device);
private:
    ::std::atomic<_OPENNUI_DEVICE state_type> _state = _OPENNUI_DEVICE state_type::unloaded;
};

_OPENNUI_DEVICE state_type test_device::open()
{
    return _OPENNUI_DEVICE state_type::error;
}

_OPENNUI_DEVICE state_type test_device::suspend()
{
    return _OPENNUI_DEVICE state_type::error;
}

_OPENNUI_DEVICE state_type test_device::release()
{
    return _OPENNUI_DEVICE state_type::error;
}

_OPENNUI_DEVICE state_type test_device::state() const
{
    return _state;
}

_OPENNUI_DEVICE string test_device::name() const
{
    _OPENNUI_DEVICE string s("undefined");
    return s;
}

_OPENNUI_DEVICE string test_device::vendor() const
{
    _OPENNUI_DEVICE string s("undefined");
    return s;
}

_OPENNUI_DEVICE string test_device::uuid() const
{
    _OPENNUI_DEVICE string s("undefined");
    return s;
}

_OPENNUI_DEVICE string test_device::rivision() const
{
    _OPENNUI_DEVICE string s("undefined");
    return s;
}

_OPENNUI bitflag test_device::databits() const
{
    return (~0) & (
        static_cast<int>(_OPENNUI_DEVICE data_bit::depth) |
        static_cast<int>(_OPENNUI_DEVICE data_bit::color) |
        static_cast<int>(_OPENNUI_DEVICE data_bit::body)  |
        static_cast<int>(_OPENNUI_DEVICE data_bit::hand)  |
        static_cast<int>(_OPENNUI_DEVICE data_bit::audio)
        );
}

void test_device::get_color_frame_info(_OPENNUI video_frame_info& out) const
{
    out.width = 0;
    out.height = 0;
    out.channel = 0;
    out.byte_per_pixel = 0;
    return;
}

void test_device::get_depth_frame_info(_OPENNUI video_frame_info& out) const
{
    out.width = 0;
    out.height = 0;
    out.channel = 0;
    out.byte_per_pixel = 0;
    return;
}

void test_device::get_body_tracking_info(_OPENNUI body_tracking_info& out) const
{
    out.maximum_tracking_bodies = 0;
    out.number_of_joints = 0;
    return;
}

bool test_device::acquire_color_frame(const _OPENNUI byte* dst)
{
    return false;
}

bool test_device::acquire_depth_frame(const _OPENNUI byte* dst)
{
    return false;
}

bool test_device::acquire_body_frame(const _OPENNUI byte* dst)
{
    return false;
}