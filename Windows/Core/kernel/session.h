#ifndef _KERNEL_SESSION_H_
#define _KERNEL_SESSION_H_

#include <vee/exception.h>
#include <memory>

namespace kernel {

class session abstract
{
public:
    
};
using session_ptr = ::std::shared_ptr<session>;

} // namespace kernel

#endif // !_KERNEL_SESSION_H_
