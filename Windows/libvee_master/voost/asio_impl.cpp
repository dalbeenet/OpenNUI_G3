#include <vee/voost/asio.h>

namespace vee {
namespace voost {

io_service_sigleton::io_service_sigleton():
_worker(this->_io_service),
_thread([this]()
{
    //! io_service.run()이 실패했을 경우는 고려되지 않음
    _io_service.run();
    while (!_sigch.try_send());
})
{
    
}

io_service_sigleton::~io_service_sigleton()
{
    _io_service.stop();
    _thread.detach();
    _sigch.recv();
}

}
}