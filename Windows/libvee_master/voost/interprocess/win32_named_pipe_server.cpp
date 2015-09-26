#include <vee/voost/win32_pipe.h>
#pragma warning(disable:4996)
namespace vee {
namespace voost {
namespace interprocess {

win32_named_pipe_acceptor::~win32_named_pipe_acceptor() __noexcept
{

}

win32_named_pipe_acceptor::win32_named_pipe_acceptor() __noexcept
{

}

win32_named_pipe_acceptor::win32_named_pipe_acceptor(win32_named_pipe_acceptor&& other)
{

}

win32_named_pipe_acceptor& win32_named_pipe_acceptor::operator=(win32_named_pipe_acceptor&& other)
{
    return *this;
}

win32_named_pipe win32_named_pipe_acceptor::accept(const string pipe_name, 
                                                   const pipe_data_transfer_mode io_mode, 
                                                   const uint32_t in_buffer_size, 
                                                   const uint32_t out_buffer_size) throw(...)
{
    DWORD win32_pipe_type_arg = NULL;
    switch (io_mode)
    {
    case pipe_data_transfer_mode::iomode_byte:
        win32_pipe_type_arg = PIPE_READMODE_BYTE | PIPE_TYPE_BYTE;
        break;
    case pipe_data_transfer_mode::iomode_message:
        win32_pipe_type_arg = PIPE_READMODE_MESSAGE | PIPE_TYPE_MESSAGE;
        break;
    default:
        throw vee::exception("Invalid parameter: pipe io_mode", (int)system::error_code::invalid_parameter);
    }
    win32_handle pipe_handle = CreateNamedPipeA(
        pipe_name.data(),    // pipe name
        PIPE_ACCESS_DUPLEX,  // read and wirte access
        win32_pipe_type_arg | PIPE_WAIT, // pipe mode | blocking_mode
        PIPE_UNLIMITED_INSTANCES,// max. instances
        in_buffer_size,
        out_buffer_size,
        NMPWAIT_USE_DEFAULT_WAIT, // client time-out
        NULL // default security attribute
        );

    if (pipe_handle == INVALID_HANDLE_VALUE)
    {
        char buffer[256] = { 0, };
        sprintf(buffer, "CreateNamedPipeA failure. GLE: %d", GetLastError());
        throw vee::exception(buffer, (int)system::error_code::server_accept_failure);
    }

    // Wait for the client to connect
    BOOL result = ConnectNamedPipe(pipe_handle.get(), NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
    if (!result)
    {
        char buffer[256] = { 0, };
        sprintf(buffer, "ConnectNamedPipe failure. GLE: %d", GetLastError());
        throw vee::exception(buffer, (int)system::error_code::server_accept_failure);
    }

    return win32_named_pipe(std::move(pipe_handle));
}

class win32_named_pipe_server: public named_pipe_server
{
    DISALLOW_COPY_AND_ASSIGN(win32_named_pipe_server);
public:
    
protected:
    
};

} // namespace interprocess
} // namespace voost
} // namespace vee