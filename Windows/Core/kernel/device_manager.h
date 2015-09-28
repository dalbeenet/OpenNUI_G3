#ifndef _OPENNUIG3_KERNEL_MODULE_MANAGER_H_
#define _OPENNUIG3_KERNEL_MODULE_MANAGER_H_

#include <kernel/device_module.h>
#include <vector>
#include <map>
#include <vee/lock.h>

namespace kernel {

class device_manager final
{
    DISALLOW_COPY_AND_ASSIGN(device_manager);
    DISALLOW_MOVE_AND_ASSIGN(device_manager);
public:
    using mutex_t = ::vee::spin_lock;
    using key_t = protocol::device_key_t;
    using device_module_ptr = ::std::shared_ptr<device_module>;
    static ::std::shared_ptr<device_manager> get_instance();
    ~device_manager();
    key_t add_module(const char* module_name) throw(...);
    void  remove_module(key_t key) throw(...);
    void  remove_module(const char* module_name) throw(...);
    device_module_ptr get_module_pointer(key_t key) throw(...);
    ::std::vector<key_t> get_all_keys();
    ::std::vector<device_module_ptr> get_all_module_pointers();
private:
    device_manager();
    mutable mutex_t _mtx;
    ::std::map<uint32_t, device_module_ptr> _modules;
};

} // namespace kernel

#endif // _OPENNUIG3_KERNEL_MODULE_MANAGER_H_