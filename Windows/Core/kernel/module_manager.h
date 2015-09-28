#ifndef _OPENNUIG3_KERNEL_MODULE_MANAGER_H_
#define _OPENNUIG3_KERNEL_MODULE_MANAGER_H_

#include <kernel/device_module.h>
#include <map>
#include <vee/lock.h>

namespace kernel {

class module_manager final
{
    DISALLOW_COPY_AND_ASSIGN(module_manager);
    DISALLOW_MOVE_AND_ASSIGN(module_manager);
public:
    using mutex_t = ::vee::spin_lock;
    using key_t = uint32_t;
    static ::std::shared_ptr<module_manager> get_instance();
    ~module_manager();
    key_t add_module(const char* module_name) throw(...);
    void  remove_module(key_t key) throw(...);
    void  remove_module(const char* module_name) throw(...);
private:
    module_manager();
    mutable mutex_t _mtx;
    ::std::map<uint32_t, ::std::shared_ptr<device_module> > _modules;
};

} // namespace kernel

#endif // _OPENNUIG3_KERNEL_MODULE_MANAGER_H_