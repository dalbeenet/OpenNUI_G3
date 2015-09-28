#include <kernel/device_manager.h>
#include <kernel/error.h>
#include <algorithm>
#pragma warning(disable:4996)
namespace kernel {

::std::shared_ptr<device_manager> device_manager::get_instance()
{
    static ::std::shared_ptr<device_manager> instance(new device_manager);
    return instance;
}

device_manager::~device_manager()
{
    //TODO: module의 안전한 삭제 - 이미 안전?
}

device_manager::device_manager()
{

}

device_manager::key_t device_manager::add_module(const char* module_name) throw(...)
{
    ::std::lock_guard<::vee::spin_lock> _guard(_mtx);
    ::std::shared_ptr<device_module> module = ::std::make_shared<device_module>(module_name);
    auto result = _modules.insert(::std::make_pair(module->key(), module));
    if (result.second == false)
    {
        char buffer[256] = { 0, };
        sprintf(buffer, "Could not add a module [%s]", module_name);
        throw vee::exception(buffer, (int)error_code::add_module_failure);
    }
    return module->key();
}

void device_manager::remove_module(key_t key) throw(...)
{
    ::std::lock_guard<::vee::spin_lock> _guard(_mtx);
     uint32_t number_of_erased = _modules.erase(key);
     if (number_of_erased == 0)
     {
         char buffer[256] = { 0, };
         sprintf(buffer, "Could not remove a module by key [%d]", key);
         throw vee::exception(buffer, (int)error_code::remove_module_failure);
     }
     printf("Remove module [%d]\n");
}

void device_manager::remove_module(const char* module_name) throw(...)
{
    ::std::string name(module_name);
    ::std::hash<::std::string> hash;
    key_t key = hash(name);
    this->remove_module(key);
}

device_manager::device_module_ptr device_manager::get_module_pointer(key_t key) throw(...)
{
    ::std::lock_guard<::vee::spin_lock> _guard(_mtx);
    auto result = _modules.find(key);
    if (result == _modules.end())
    {
        char buffer[256] = { 0, };
        sprintf(buffer, "Could not find a module by key [%d]", key);
        throw vee::exception(buffer, (int)error_code::find_module_failure);
    }
    return result->second;
}

::std::vector<protocol::device_key_t> device_manager::get_all_keys()
{
    ::std::lock_guard<::vee::spin_lock> _guard(_mtx);
    ::std::vector<protocol::device_key_t> result;
    for (auto& iter : _modules)
    {
        result.push_back(iter.first);
    }
    return result;
}

::std::vector<device_manager::device_module_ptr> device_manager::get_all_module_pointers()
{
    ::std::lock_guard<::vee::spin_lock> _guard(_mtx);
    ::std::vector<device_manager::device_module_ptr> result;
    for (auto& iter : _modules)
    {
        result.push_back(iter.second);
    }
    return result;
}

} // namespace kernel