#include <kernel/shared_buffer.h>
#include <algorithm>

namespace kernel {

shared_buffer_ptr shared_buffer::crate(const char* shm_name, uint32_t size, protocol::device_key_t device_key, protocol::session_key_t session_key, protocol::frame_type data_type)
{
    return ::std::make_shared<shared_buffer>(shm_name, size, device_key, session_key, data_type);
}

shared_buffer::~shared_buffer()
{
    //puts(__FUNCTION__);
}

shared_buffer::shared_buffer(const char* shm_name, uint32_t size, protocol::device_key_t device_key, protocol::session_key_t session_key, protocol::frame_type data_type):
_data_type(data_type),
_device_key(device_key),
_session_key(session_key),
_shm(nullptr),
_key(0)
{
    _shm = ::vee::voost::create_shared_memory(shm_name, size);
    memset(_shm->get_address(), 0, size);
    ::std::string name(shm_name);
    ::std::hash<::std::string> hash;
    _key = hash(name);
}

} // namespace kernel