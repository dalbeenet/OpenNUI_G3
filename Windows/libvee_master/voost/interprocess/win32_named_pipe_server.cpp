#include <vee/voost/win32_pipe.h>
#pragma warning(disable:4996)
namespace vee {
namespace voost {
namespace interprocess {
namespace win32 {

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

win32_named_pipe win32_named_pipe_acceptor::accept(const char* pipe_name,
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
        pipe_name,    // pipe name
        PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,  // read and wirte access
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

    win32_named_pipe pipe(pipe_handle.get(), pipe_name, true);
    pipe_handle.clear();
    return pipe;
}

//////////////////////////////////////////////////////////////////////////

win32_named_pipe_server::~win32_named_pipe_server() __noexcept
{

}

win32_named_pipe_server::win32_named_pipe_server() __noexcept
{

}

win32_named_pipe_server::win32_named_pipe_server(win32_named_pipe_server&& other) __noexcept
{

}

win32_named_pipe_server& win32_named_pipe_server::operator=(win32_named_pipe_server&& other) __noexcept
{
    return *this;
}

win32_named_pipe_server::generic_session_ptr win32_named_pipe_server::accept(const char* pipe_name, const pipe_data_transfer_mode mode, const uint32_t in_buffer_size, const uint32_t out_buffer_size) throw(...)
{
    generic_session_ptr generic_session = std::make_shared<session_t>(_acceptor.accept(pipe_name, mode, in_buffer_size, out_buffer_size));
    return generic_session;
}

void win32_named_pipe_server::close() __noexcept
{

}

::std::shared_ptr<named_pipe_server> create_named_pipe_server() __noexcept
{
    ::std::shared_ptr<named_pipe_server> server = ::std::make_shared<win32_named_pipe_server>();
    return server;
}

} // namespace win32
} // namespace interprocess
} // namespace voost
} // namespace vee