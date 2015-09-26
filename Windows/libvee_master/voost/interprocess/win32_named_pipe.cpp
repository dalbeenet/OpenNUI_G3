#include <vee/voost/win32_pipe.h>

#pragma warning(disable:4996)
namespace vee {
namespace voost {
namespace interprocess {
namespace win32 {

win32_named_pipe::~win32_named_pipe() __noexcept
{
    disconnect();
}

win32_named_pipe::win32_named_pipe() __noexcept
{
    
}

win32_named_pipe::win32_named_pipe(win32_handle&& handle, const char* pipe_name, bool is_server_side) __noexcept:
_pipe_handle(std::move(handle)),
_pipe_name(pipe_name),
_is_server_side(is_server_side)
{

}

win32_named_pipe::win32_named_pipe(win32_named_pipe&& other) __noexcept:
_pipe_handle(std::move(other._pipe_handle)),
_pipe_name(std::move(other._pipe_name)),
_is_server_side(other._is_server_side)
{

}

win32_named_pipe& win32_named_pipe::operator=(win32_named_pipe&& other) __noexcept
{
    _pipe_handle = std::move(other._pipe_handle);
    _pipe_name = std::move(other._pipe_name);
    _is_server_side = other._is_server_side;
    return *this;
}

void win32_named_pipe::connect(const char* pipe_name, const creation_option creation_opt, const pipe_data_transfer_mode read_mode, const uint32_t time_out_millisec) throw(...)
{
    if (_pipe_handle.get() != NULL)
        throw vee::exception("pipe is already opened!", (int)error_code::stream_already_connected);
    HANDLE pipe_handle = NULL;
    {
        DWORD win32_file_craetion_opt = NULL;
        switch (creation_opt)
        {
        case creation_option::create_or_truncate:
            win32_file_craetion_opt = CREATE_ALWAYS;
            break;
        case creation_option::create_new:
            win32_file_craetion_opt = CREATE_NEW;
            break;
        case creation_option::open_or_crate:
            win32_file_craetion_opt = OPEN_ALWAYS;
            break;
        case creation_option::open_existing:
            win32_file_craetion_opt = OPEN_EXISTING;
            break;
        case creation_option::truncate_existing:
            win32_file_craetion_opt = TRUNCATE_EXISTING;
            break;
        default:
            throw vee::exception("Invalid parameter: creation_option", (int)system::error_code::invalid_parameter);
        }

        pipe_handle = CreateFileA(
            pipe_name, // pipe name
            GENERIC_READ | GENERIC_WRITE, // read and write access
            0,              // no sharing
            NULL,           // default secuirty attributes
            win32_file_craetion_opt,  // creation option
            0,              // defualt attributes
            NULL            // no template file
            );

        // get an error code
        DWORD error = GetLastError();

        // Check the piple handle validity
        if (pipe_handle == INVALID_HANDLE_VALUE && error != ERROR_PIPE_BUSY)
        {
            // Throw an exception if an error other than ERROR_PIPE_BUSY occurs.    
            char buffer[256] = { 0, };
            sprintf(buffer, "CreateFileA(Win32 api) failure! GLE: %d", error);
            throw vee::exception(buffer, (int)system::error_code::stream_connection_failure);
        }
        else if (error == ERROR_PIPE_BUSY)
        {
            // If a pipe instance is busy, so wait for timeout limit.
            DWORD time_out_arg = 0;
            switch (time_out_millisec)
            {
            case pipe_time_out_constant::wait_default:
                time_out_arg = NMPWAIT_USE_DEFAULT_WAIT;
                break;
            case pipe_time_out_constant::wait_forever:
                time_out_arg = NMPWAIT_WAIT_FOREVER;
                break;
            case pipe_time_out_constant::wait_nowait:
                time_out_arg = NMPWAIT_NOWAIT;
                break;
            default:
                time_out_arg = time_out_millisec;
                break;
            }
            if (!WaitNamedPipeA(pipe_name, time_out_arg))
            {
                char buffer[256] = { 0, };
                switch (time_out_millisec)
                {
                case pipe_time_out_constant::wait_default:
                    sprintf(buffer, "Could not open pipe GLE=%d \"%s\", pipe_time_out_constant::wait_default.", GetLastError(), pipe_name);
                    break;
                case pipe_time_out_constant::wait_forever:
                    sprintf(buffer, "Could not open pipe GLE=%d \"%s\", pipe_time_out_constant::wait_forever.", GetLastError(), pipe_name);
                    break;
                case pipe_time_out_constant::wait_nowait:
                    sprintf(buffer, "Could not open pipe GLE=%d \"%s\", pipe_time_out_constant::wait_nowait.", GetLastError(), pipe_name);
                    break;
                default:
                    sprintf(buffer, "Could not open pipe GLE=%d \"%s\", %.1f second wait timed out.", GetLastError(), pipe_name, (float)(time_out_millisec / 1000));
                    break;
                }
                // Throw an exception if timed out.
                throw vee::exception(buffer, (int)system::error_code::win32_busy_named_pipe);
            }
        }
    }

    // The pipe connected, change to the read mode
    {
        DWORD win32_pipe_read_mode = NULL;
        switch (read_mode)
        {
        case pipe_data_transfer_mode::iomode_byte:
            win32_pipe_read_mode = PIPE_READMODE_BYTE;
            break;
        case pipe_data_transfer_mode::iomode_message:
            win32_pipe_read_mode = PIPE_READMODE_MESSAGE;
            break;
        default:
            throw vee::exception("Invalid parameter: pipe_read_mode", (int)system::error_code::invalid_parameter);
        }

        BOOL result = SetNamedPipeHandleState(
            pipe_handle,    // pipe handle 
            &win32_pipe_read_mode,  // new pipe mode 
            NULL,     // don't set maximum bytes 
            NULL);    // don't set maximum time 
        if (!result)
        {
            char buffer[256] = { 0, };
            sprintf(buffer, "SetNamedPipeHandleState failed. GLE=%d", GetLastError());
            throw vee::exception(buffer, (int)system::error_code::stream_connection_failure);
        }
    }

    // pipe stream ready
    _pipe_handle = pipe_handle;
    _pipe_name = pipe_name;
    _is_server_side = false;
}

void win32_named_pipe::disconnect() __noexcept
{
    if (_is_server_side)
    {
        FlushFileBuffers(_pipe_handle.get());
        DisconnectNamedPipe(_pipe_handle.get());
    }
    _pipe_handle.close_nothrow();
    _is_server_side = false;
    _pipe_handle = NULL;
}

uint32_t win32_named_pipe::read(byte* const buffer, const uint32_t buf_capacity) throw(...)
{
    if (_pipe_handle == (HANDLE)NULL)
    {
        throw vee::exception("Invalid named pipe stream", (int)system::error_code::closed_stream);
    }
    DWORD bytes_transferred = 0;
    BOOL result = ReadFile(_pipe_handle.get(),      // pipe handle
                           buffer,                  // buffer to receive reply
                           buf_capacity,            // size of buffer
                           &bytes_transferred,      // number of bytes read
                           NULL);                   // not overlapped
    if (!result)
    {
        char temp[256] = { 0, };
        switch (GetLastError())
        {
        case 109:
            sprintf(temp, "ReadFile from pipe failed. The named pipe stream is closed by host! GLE=%d", GetLastError());
            throw vee::exception(temp, (int)system::error_code::stream_disconnected_by_host);
        default:
            sprintf(temp, "ReadFile from pipe failed. GLE=%d", GetLastError());
            throw vee::exception(temp, (int)system::error_code::stream_send_failure);
        }
    }
    else if (GetLastError() == ERROR_MORE_DATA)
    {
        char temp[256] = { 0, };
        sprintf(temp, "Could not copy all of received data to receive buffer. GLE=%d", GetLastError());
        throw vee::exception(temp, (int)system::error_code::buffer_is_too_small);
    }

    return bytes_transferred;
}

uint32_t win32_named_pipe::write(const byte* data, const uint32_t size) throw(...)
{
    if (_pipe_handle == (HANDLE)NULL)
    {
        throw vee::exception("Invalid named pipe stream", (int)system::error_code::closed_stream);
    }
    DWORD bytes_transferred = 0;
    BOOL result = WriteFile(_pipe_handle.get(),     // pipe handle
                            data,                   // data buffer
                            size,                   // length of data
                            &bytes_transferred,     // bytes written
                            NULL);                  // not overlapped
    if (!result)
    {
        char temp[256] = { 0, };
        sprintf(temp, "WriteFile to pipe failed. GLE=%d", GetLastError());
        throw vee::exception(temp, (int)system::error_code::stream_send_failure);
    }

    return static_cast<uint8_t>(bytes_transferred);
}

::std::shared_ptr<named_pipe> create_named_pipe()
{
    ::std::shared_ptr<named_pipe> pipe = ::std::make_shared<win32_named_pipe>();
    return pipe;
}

} // namespace win32
} // namespace interprocess
} // namespace voost
} // namespace vee
#pragma warning(default:4996)