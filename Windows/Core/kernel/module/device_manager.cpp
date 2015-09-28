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
    //TODO: module의 안전한 삭제
}

device_manager::device_manager()
{

}

device_manager::key_t device_manager::add_module(const char* module_name) throw(...)
{
    ::std::lock_guard<::vee::spin_lock> _guard(_mtx);
    ::std::string name(module_name);
    ::std::hash<::std::string> hash;
    key_t key = hash(name);
    ::std::shared_ptr<device_module> module = ::std::make_shared<device_module>(module_name);
    auto result = _modules.insert(::std::make_pair(key, module));
    if (result.second == false)
    {
        char buffer[256] = { 0, };
        sprintf(buffer, "Could not add a module [%s]", module_name);
        throw vee::exception(buffer, (int)error_code::add_module_failure);
    }
    return key;
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
}

void device_manager::remove_module(const char* module_name) throw(...)
{
    ::std::string name(module_name);
    ::std::hash<::std::string> hash;
    key_t key = hash(name);
    this->remove_module(key);
}

} // namespace kernel