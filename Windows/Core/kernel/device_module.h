#ifndef _KERNEL_DEVICE_MODULE_H_
#define _KERNEL_DEVICE_MODULE_H_

#include <vee/macro.h>
#include <vee/win32.h>
#include <opennui/opennui.h>
#include <memory>
#include <string>

namespace kernel {

class device_module
{
    DISALLOW_COPY_AND_ASSIGN(device_module);
    using string = ::std::string;
public:
    using opennui_device = _OPENNUI opennui_device;
    using opennui_device_ptr = ::std::shared_ptr<opennui_device>;
    ~device_module() throw(...);
    device_module(const char* module_name);
    device_module(device_module&& other);
    device_module& operator=(device_module&& other);
protected:
    string _module_name;
    opennui_device_ptr _device_ptr;
    HMODULE _module_handle;
};

} // namespace kernel

#endif // !_KERNEL_DEVICE_MODULE_H_
