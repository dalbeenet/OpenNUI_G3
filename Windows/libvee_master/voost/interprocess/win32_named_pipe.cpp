#include <vee/voost/pipe.h>
#include <vee/macro.h>
#include <vee/exception.h>
#include <Windows.h>
#pragma warning(disable:4996)
namespace vee {
namespace voost {
namespace interprocess {

class win32_named_pipe: named_pipe
{
    DISALLOW_COPY_AND_ASSIGN(win32_named_pipe);
public:
    ~win32_named_pipe();
    win32_named_pipe();
    virtual void connect(const char* pipe_name, const creation_option creation_opt, const pipe_read_mode read_mode, const uint32_t time_out_millisec) throw(...) override;
    virtual uint32_t write(const byte* data, const uint32_t size) throw(...) override;
    virtual uint32_t read(byte* const buffer, const uint32_t buf_capacity) throw(...) override;
protected:
    HANDLE _pipe_handle;
    string _pipe_name;
};

win32_named_pipe::~win32_named_pipe()
{

}

win32_named_pipe::win32_named_pipe():
_pipe_handle(NULL),
_pipe_name("")
{
        
}

void win32_named_pipe::connect(const char* pipe_name, const creation_option creation_opt, const pipe_read_mode read_mode, const uint32_t time_out_millisec) throw(...)
{
    if (_pipe_handle != NULL)
        throw vee::exception("pipe is already opened!", (int)error_code::stream_already_connected);

    DWORD creation_disposition = NULL;
    switch (creation_opt)
    {
    case creation_option::create_or_truncate:
        creation_disposition = CREATE_ALWAYS;
        break;
    case creation_option::create_new:
        creation_disposition = CREATE_NEW;
        break;
    case creation_option::open_or_crate:
        creation_disposition = OPEN_ALWAYS;
        break;
    case creation_option::open_existing:
        creation_disposition = OPEN_EXISTING;
        break;
    case creation_option::truncate_existing:
        creation_disposition = TRUNCATE_EXISTING;
        break;
    }
    
    _pipe_handle = CreateFileA(
        pipe_name, // pipe name
        GENERIC_READ | GENERIC_WRITE, // read and write access
        0,              // no sharing
        NULL,           // default secuirty attributes
        creation_disposition,  // creation option
        0,              // defualt attributes
        NULL            // no template file
        );

    // return function if the pipe handle is valid (success)
    if (_pipe_handle != INVALID_HANDLE_VALUE)
        return;

    // get an error code
    DWORD error = GetLastError();
    // Throw an exception if an error other than ERROR_PIPE_BUSY occurs.    
    if (error != ERROR_PIPE_BUSY)
    {
        char buffer[128] = { 0, };
        sprintf(buffer, "CreateFileA(Win32 api) failure! win32_error_code: %d", error);
        throw vee::exception(buffer, (int)system::error_code::stream_connection_failure);
    }

    // All pipe instances are busy, so wait for timeout limit.
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
        char buffer[128] = { 0, };
        switch (time_out_millisec)
        {
        case pipe_time_out_constant::wait_default:
            sprintf(buffer, "Could not open pipe \"%s\", pipe_time_out_constant::wait_default.", pipe_name);
            break;
        case pipe_time_out_constant::wait_forever:
            sprintf(buffer, "Could not open pipe \"%s\", pipe_time_out_constant::wait_forever.", pipe_name);
            break;
        case pipe_time_out_constant::wait_nowait:
            sprintf(buffer, "Could not open pipe \"%s\", pipe_time_out_constant::wait_nowait.", pipe_name);
            break;
        default:
            sprintf(buffer, "Could not open pipe \"%s\", %.1f second wait timed out.", pipe_name, time_out_millisec / 1000);
            break;
        }
        // Throw an exception if timed out.
        throw vee::exception(buffer, (int)system::error_code::win32_busy_named_pipe);
    }

    // Sets the read mode and the bloking mode of the specified named pipe. 
    // If the specified handle is to the client end of a named pipe
    // and if the named pipe server process is on a remote computer, 
    // the function can also be used to control local buffering.
    
}

} // namespace interprocess
} // namespace voost
} // namespace vee
#pragma warning(default:4996)