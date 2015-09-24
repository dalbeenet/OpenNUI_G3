#include <vee/voost/pipe.h>
#include <vee/macro.h>
#include <vee/exception.h>
#include <Windows.h>

namespace vee {
namespace voost {
namespace interprocess {

class win32_named_pipe: named_pipe
{
    DISALLOW_COPY_AND_ASSIGN(win32_named_pipe);
public:
    ~win32_named_pipe();
    win32_named_pipe();
    virtual void connect(const char* pipe_name, const creation_option creation_opt) throw(...) override;
    virtual uint32_t write(const byte* data, const uint32_t size) throw(...) override;
    virtual uint32_t read(byte* const buffer, const uint32_t buf_capacity) throw(...) override;
protected:
    HANDLE _pipe_handle;
    string _pipe_name;
};

win32_named_pipe::~win32_named_pipe()
{

}

win32_named_pipe::win32_named_pipe()
{
        
}

void win32_named_pipe::connect(const char* pipe_name, const creation_option creation_opt) throw(...)
{
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
        sprintf(buffer, "CreateFileA(Win32 api) failure! error_code: %d", );
        throw vee::exception(buffer, (int)system::error_code::stream_connection_failure);
    }
}

} // namespace interprocess
} // namespace voost
} // namespace vee