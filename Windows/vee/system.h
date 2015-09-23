#ifndef _VEE_SYSTEM_H_
#define _VEE_SYSTEM_H_

namespace vee {
namespace system {

enum class error_code: unsigned int
{
    success,
};

struct operation_result
{
    error_code error;
};

} // namespace system
} // namespace vee

#endif // !_VEE_SYSTEM_H_
