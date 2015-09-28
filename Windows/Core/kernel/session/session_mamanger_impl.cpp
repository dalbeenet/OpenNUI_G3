#include <kernel/session_manager.h>
#include <kernel/error.h>
#pragma warning(disable:4996)
namespace kernel {

::std::shared_ptr<session_manager> session_manager::get_instance()
{
    static ::std::shared_ptr<session_manager> instance(new session_manager);
    return instance;
}

session_manager::~session_manager()
{
    
}

session_manager::session_manager()
{

}

session_manager::key_t session_manager::add_session(session_ptr sptr, key_t sid) throw(...)
{
    ::std::lock_guard<::vee::spin_lock> _guard(_mtx);
    auto result = _sessions.insert(::std::make_pair(sid, sptr));
    if (result.second == false)
    {
        char buffer[256] = { 0, };
        sprintf(buffer, "Could not add a session [%d]", sid);
        throw vee::exception(buffer, (int)error_code::add_session_failure);
    }
    return sid;
}

void session_manager::remove_session(key_t key) throw(...)
{
    ::std::lock_guard<::vee::spin_lock> _guard(_mtx);
    uint32_t number_of_erased = _sessions.erase(key);
    if (number_of_erased == 0)
    {
        char buffer[256] = { 0, };
        sprintf(buffer, "Could not remove a session by key [%d]", key);
        throw vee::exception(buffer, (int)error_code::remove_module_failure);
    }
}

void session_manager::remove_session(session_ptr sptr) throw(...)
{
    key_t key = sptr->get_id();
    this->remove_session(key);
}

} // namespace kernel