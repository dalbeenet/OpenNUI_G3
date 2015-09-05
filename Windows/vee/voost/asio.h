#ifndef _VEE_VOOST_ASIO_H_
#define _VEE_VOOST_ASIO_H_

#include <boost/asio.hpp>
#include <boost/serialization/singleton.hpp>
#include <vee/signal_channel.h>

namespace vee {
namespace voost {

class io_service_sigleton: public boost::serialization::singleton<io_service_sigleton>
{
    friend class boost::serialization::singleton<io_service_sigleton>;
    DISALLOW_COPY_AND_ASSIGN(io_service_sigleton);
    DISALLOW_MOVE_AND_ASSIGN(io_service_sigleton);
public:
    using io_service_t = ::boost::asio::io_service;
    using worker_t = boost::asio::io_service::work;
    io_service_sigleton();
    ~io_service_sigleton();
    inline static io_service_sigleton& get()
    {
        return boost::serialization::singleton<io_service_sigleton>::get_mutable_instance();
    }
    inline io_service_t& io_service()
    {
        return _io_service;
    }
private:
    io_service_t _io_service;
    worker_t _worker;
    ::std::thread _thread;
    alram_channel _sigch;
};

} // namespace voost
} // namespace vee

#endif // _VEE_VOOST_ASIO_H_