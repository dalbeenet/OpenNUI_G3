#ifndef _OPENNUI_G3_KERNEL_SESSION_MANAGER_H_
#define _OPENNUI_G3_KERNEL_SESSION_MANAGER_H_

#include <kernel/session.h>
#include <vector>
#include <vee/lock.h>

namespace kernel {

class session_manager final
{
    DISALLOW_COPY_AND_ASSIGN(session_manager);
    DISALLOW_MOVE_AND_ASSIGN(session_manager);
public:
    using mutex_t = ::vee::spin_lock;
    static ::std::shared_ptr<session_manager> get_instance();
    ~session_manager();
private:
    session_manager();
    mutable mutex_t _mtx;
    ::std::vector< ::std::shared_ptr<session> > _modules;
};

} // namespace kernel

#endif // _OPENNUI_G3_KERNEL_SESSION_MANAGER_H_