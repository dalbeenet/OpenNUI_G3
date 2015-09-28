#include <kernel/session_manager.h>

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

} // namespace kernel