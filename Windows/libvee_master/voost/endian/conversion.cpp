#include <vee/voost/endian.h>
#include <boost/endian/conversion.hpp>

namespace vee {

uint64_t htonll(uint64_t value)
{
    uint64_t result = ::boost::endian::big_to_native(value);
    return result;
}

} // namespace vee