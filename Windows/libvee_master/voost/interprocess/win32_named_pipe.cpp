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

        // Check the piple handle validity
        if (pipe_handle == INVALID_HANDLE_VALUE)
        {
            // get an error code
            DWORD error = GetLastError();
            // Throw an exception if an error other than ERROR_PIPE_BUSY occurs.    
            if (error != ERROR_PIPE_BUSY)
            {
                char buffer[128] = { 0, };
                sprintf(buffer, "CreateFileA(Win32 api) failure! win32_error_code: %d", error);
                throw vee::exception(buffer, (int)system::error_code::stream_connection_failure);
            }
        }
        else
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
        }
    }

    // The pipe connected, change to the read mode
    {
        DWORD win32_pipe_read_mode = NULL;
        switch (read_mode)
        {
        case pipe_read_mode::readmode_byte:
            win32_pipe_read_mode = PIPE_READMODE_BYTE;
            break;
        case pipe_read_mode::readmode_message:
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

}

} // namespace interprocess
} // namespace voost
} // namespace vee
#pragma warning(default:4996)