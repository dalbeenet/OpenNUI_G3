#ifndef _VEE_VOOST_SHARED_MEMORY_H_
#define _VEE_VOOST_SHARED_MEMORY_H_

#include <vee/macro.h>
#include <memory>

_VEE_BEGIN
_VOOST_BEGIN

class shared_memory abstract
{
public:
    enum class create_option: int
    {
        create_only = 0,
        //open_only,
        //open_read_only,
        open_or_create,
        //open_copy_on_write
    };
    enum class authority: int
    {
        //TODO: IMPL    
    };
    using size_t = unsigned int;
    virtual ~shared_memory() = default;
    virtual void* get_address() const = 0;
    virtual size_t get_size() const = 0;
    virtual const char* get_name() const = 0;
};

::std::shared_ptr<shared_memory> create_shared_memory(const char* shm_name, shared_memory::size_t size, shared_memory::create_option create_opt = shared_memory::create_option::open_or_create);

_VOOST_END
_VEE_END

#endif // _VEE_COMMON_SHARED_MEMORY_H_