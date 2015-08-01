#ifndef _VEE_VOOST_SHARED_MEMORY_H_
#define _VEE_VOOST_SHARED_MEMORY_H_

#include <vee/macro.h>
#if VEE_PLATFORM_WINDOWS
#include <boost/interprocess/windows_shared_memory.hpp>
#else
// Boost c++ shared memomy header. (BOOST 1.58.0)
#include <boost/interprocess/shared_memory_object.hpp>
#endif
#include <boost/interprocess/mapped_region.hpp>
#include <cstdlib>
#include <string>

_VEE_BEGIN
_VOOST_BEGIN

class shared_memory
{
public:
    static const ::boost::interprocess::create_only_t         create_only;
    static const ::boost::interprocess::open_only_t           open_only;
    static const ::boost::interprocess::open_read_only_t      open_read_only;
    static const ::boost::interprocess::open_or_create_t      open_or_create;
    static const ::boost::interprocess::open_copy_on_write_t  open_copy_on_write;
    typedef      ::boost::interprocess::mode_t access_mode;
    typedef      ::boost::interprocess::mapped_region mapped_region;
    typedef      ::boost::interprocess::offset_t offset_type;
#if VEE_PLATFORM_WINDOWS
    typedef      ::boost::interprocess::windows_shared_memory shared_memory_type;
#else
    typedef      ::boost::interprocess::shared_memory_object shared_memory_type;
#endif
    //TODO: copy ctor and move ctor and operator =
    // ctor (template for optional)
    template <class CreateOpt>
    shared_memory(const char* name, const int size, CreateOpt, access_mode mode):
        shm_name(name),
        shm_size(size),
        shm(CreateOpt(),    
        name,
        mode
#if VEE_PLATFORM_WINDOWS
        , size)
#else
        )
#endif
    {
        // nothing to do.   
    }
    ~shared_memory()
    {
#if VEE_PLATFORM_WINDOWS
        this->remove(shm_name.data());
#endif
    }
    mapped_region get_region(offset_type offset, ::std::size_t size, access_mode mode)
    {
        // Mapping Shared Memory Segments
        // Once created or opened, a process just has to map the shared memory object in the process's address space.
        // The user can map the whole shared memory or just part of it.
        // The mapping process is done using the boost::interprocess::mapped_region class.
        // The class represents a memory region that has been mapped from a shared memory or from other devices that have also mapping capabilities. (for example, files).
        // A mapped_region can be created from any memory_mappable object and as you might imagine, shared_memory_object is a memory_mappable object
        boost::interprocess::mapped_region region(
            shm,
            mode,
            offset,
            size
            );
        return region;
    }
    static bool remove(const char* name)
    {
#if VEE_PLATFORM_WINDOWS
        return false;
#else
        return boost::interprocess::shared_memory_object::remove(name);
#endif
    }
public:
    const ::std::string shm_name;
    const ::std::size_t shm_size;
    const shared_memory_type shm;
};

_VOOST_END
_VEE_END

#endif // _VEE_COMMON_SHARED_MEMORY_H_