#include <kernel/session_manager.h>
#include <kernel/device_manager.h>
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

session_manager::key_t session_manager::add_session(session_ptr s) throw(...)
{
    ::std::lock_guard<::vee::spin_lock> _guard(_mtx);
    key_t sid = s->get_id();
    auto result = _sessions.insert(::std::make_pair(sid, s));
    if (result.second == false)
    {
        char buffer[256] = { 0, };
        sprintf(buffer, "Could not add a session [%d]", sid);
        throw vee::exception(buffer, (int)error_code::add_session_failure);
    }
    printf("system> new client connected! sid: %d\n", s->get_id());
    auto life_stream = s->get_life_stream();
    life_stream->async_read(s->_lifestream_in_buffer.data(), 
                            s->_lifestream_in_buffer.size(), 
                            ::std::bind(&session_manager::_on_session_disconnect, this, s, ::std::placeholders::_1, ::std::placeholders::_2, ::std::placeholders::_3, ::std::placeholders::_4));
    s->_launch_api_service();
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

void session_manager::_on_session_disconnect(session_ptr s,
                                             ::vee::system::operation_result& result, 
                                             ::vee::byte* const buffer, 
                                             uint32_t buf_capacity, 
                                             uint32_t byte_transferred)
{
    if (result.error == ::vee::system::error_code::success)
    {
        printf("system> protocol mismatch! %d bytes transferred from life stream.\n", byte_transferred);
        auto life_stream = s->get_life_stream();
        life_stream->async_read(s->_lifestream_in_buffer.data(),
                                s->_lifestream_in_buffer.size(),
                                ::std::bind(&session_manager::_on_session_disconnect, this, s, ::std::placeholders::_1, ::std::placeholders::_2, ::std::placeholders::_3, ::std::placeholders::_4));
        return;
    }
    try
    {
        auto device_manager = device_manager::get_instance();
        printf("system> remove the color shared buffer process begin...\n");
        {
            auto removable_buffers = s->color_buffer_table.get_all_value_copies();
            for (auto& it : removable_buffers)
            {
                try
                {
                    auto module = device_manager->get_module_pointer(it->device_key());
                    if (module->color_buffer_table.remove(it->key()))
                        printf("system> success to remove the shared_buffer named %s instance from the table. (did: %d)\n", it->name().data(), module->key());
                    else
                        printf("system> failed to remove the shared_buffer named %s instance from the table. (did: %d)\n", it->name().data(), module->key());
                }
                catch (vee::exception& e)
                {
                    printf("system> unhandled exception: %s\n", e.what());
                }
            }
        }
        printf("system> remove the depth shared buffer process begin...\n");
        {
            auto removable_buffers = s->depth_buffer_table.get_all_value_copies();
            for (auto& it : removable_buffers)
            {
                try
                {
                    auto module = device_manager->get_module_pointer(it->device_key());
                    if (module->depth_buffer_table.remove(it->key()))
                        printf("system> success to remove the shared_buffer named %s instance from the table. (did: %d)\n", it->name().data(), module->key());
                    else
                        printf("system> failed to remove the shared_buffer named %s instance from the table. (did: %d)\n", it->name().data(), module->key());
                }
                catch (vee::exception& e)
                {
                    printf("system> ynhandled exception: %s\n", e.what());
                }
            }
        }
        printf("system> remove the body shared buffer process begin...\n");
        {
            auto removable_buffers = s->body_buffer_table.get_all_value_copies();
            for (auto& it : removable_buffers)
            {
                try
                {
                    auto module = device_manager->get_module_pointer(it->device_key());
                    if (module->body_buffer_table.remove(it->key()))
                        printf("system> success to remove the shared_buffer named %s instance from the table. (did: %d)\n", it->name().data(), module->key());
                    else
                        printf("system> failed to remove the shared_buffer named %s instance from the table. (did: %d)\n", it->name().data(), module->key());
                }
                catch (vee::exception& e)
                {
                    printf("system> unhandled exception: %s\n", e.what());
                }
            }
        }

        this->remove_session(s->get_id());
        printf("system> session %d is disconnected successfully.\n", s->get_id());
    }
    catch (vee::exception& e)
    {
        printf("system> ynhandled exception: %s\n", e.what());
    }
}

} // namespace kernel