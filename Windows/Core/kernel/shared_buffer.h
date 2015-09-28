#ifndef _OPENNUIG3_KERNEL_SHARED_BUFFER_H_
#define _OPENNUIG3_KERNEL_SHARED_BUFFER_H_

#include <kernel/protocol.h>
#include <vee/voost/shared_memory.h>

namespace kernel {

class shared_buffer;
using shared_buffer_ptr = ::std::shared_ptr<shared_buffer>;

class shared_buffer
{
    DISALLOW_COPY_AND_ASSIGN(shared_buffer);
    DISALLOW_MOVE_AND_ASSIGN(shared_buffer);
public:
    using key_t = protocol::session_key_t;
    using shm_t = ::vee::voost::shared_memory;
    using shm_ptr = ::std::shared_ptr<shm_t>;
    static shared_buffer_ptr crate(const char* shm_name, uint32_t size);
    ~shared_buffer();
    shared_buffer(const char* shm_name, uint32_t size);
    inline ::std::string name()
    {
        ::std::string s(_shm->get_name());
        return s;
    }
    inline void* addr()
    {
        return _shm->get_address();
    }
    inline key_t key()
    {
        return _key;
    }
private:
    key_t   _key;
    shm_ptr _shm;
};

} // namespace kernel

#endif // !_OPENNUIG3_KERNEL_SHARED_BUFFER_H_
