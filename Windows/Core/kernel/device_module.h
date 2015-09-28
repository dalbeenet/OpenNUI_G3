#ifndef _KERNEL_DEVICE_MODULE_H_
#define _KERNEL_DEVICE_MODULE_H_

#include <vee/macro.h>
#include <vee/win32.h>
#include <kernel/protocol.h>
#include <opennui/opennui.h>
#include <memory>
#include <string>

namespace kernel {

class device_module
{
    DISALLOW_COPY_AND_ASSIGN(device_module);
    using key_t = protocol::device_key_t;
    using string = ::std::string;
public:
    using opennui_device = _OPENNUI opennui_device;
    using opennui_device_ptr = ::std::shared_ptr<_OPENNUI opennui_device>;
    ~device_module();
    device_module(const char* module_name) throw(...);
    device_module(device_module&& other);
    device_module& operator=(device_module&& other);
    inline key_t key()
    {
        return _key;
    };
    inline opennui_device_ptr opennui_device_instance()
    {
        return _device_ptr;
    }
protected:
    key_t  _key;
    string _module_name;
    opennui_device_ptr _device_ptr;
    HMODULE _module_handle;
};

} // namespace kernel

#endif // !_KERNEL_DEVICE_MODULE_H_
