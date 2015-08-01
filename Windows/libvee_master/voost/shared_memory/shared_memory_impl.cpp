#include <vee/voost/shared_memory.h>

_VEE_BEGIN
_VOOST_BEGIN

const ::boost::interprocess::create_only_t         shared_memory::create_only = ::boost::interprocess::create_only_t();
const ::boost::interprocess::open_only_t           shared_memory::open_only = ::boost::interprocess::open_only_t();
const ::boost::interprocess::open_read_only_t      shared_memory::open_read_only = ::boost::interprocess::open_read_only_t();
const ::boost::interprocess::open_or_create_t      shared_memory::open_or_create = ::boost::interprocess::open_or_create_t();
const ::boost::interprocess::open_copy_on_write_t  shared_memory::open_copy_on_write = ::boost::interprocess::open_copy_on_write_t();

_VOOST_END
_VEE_END