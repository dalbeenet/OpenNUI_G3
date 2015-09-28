#ifndef _OPENNUI_G3_KERNEL_SESSION_MANAGER_H_
#define _OPENNUI_G3_KERNEL_SESSION_MANAGER_H_

#include <kernel/session.h>
#include <map>
#include <vee/lock.h>

namespace kernel {

class session_manager final
{
    DISALLOW_COPY_AND_ASSIGN(session_manager);
    DISALLOW_MOVE_AND_ASSIGN(session_manager);
public:
    using mutex_t = ::vee::spin_lock;
    using key_t = session::session_id_t;
    static ::std::shared_ptr<session_manager> get_instance();
    ~session_manager();
    key_t add_session(session_ptr sptr) throw(...);
    void  remove_session(key_t key) throw(...);
    void  remove_session(session_ptr sptr) throw(...);
private:
    void _on_session_disconnect(session_ptr s,
                                ::vee::system::operation_result& result,
                                ::vee::byte* const buffer,
                                uint32_t buf_capacity,
                                uint32_t byte_transferred);
    session_manager();
    mutable mutex_t _mtx;
    ::std::map< key_t, session_ptr > _sessions;
};

} // namespace kernel

#endif // _OPENNUI_G3_KERNEL_SESSION_MANAGER_H_