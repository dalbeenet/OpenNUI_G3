#ifndef _OPENNUIG3_KERNEL_SESSION_WIN32_SESSION_H_
#define _OPENNUIG3_KERNEL_SESSION_WIN32_SESSION_H_
#include <kernel/error.h>
#include <kernel/session.h>
#include <vee/voost/pipe.h>

namespace kernel {

class win32_session: public session
{
    DISALLOW_COPY_AND_ASSIGN(win32_session);
public:
    using win32_message_stream = ::std::shared_ptr<::vee::voost::interprocess::named_pipe>;
    static ::std::shared_ptr<win32_session> handshake(life_stream raw_stream, session_id_t sid) throw(...);
    virtual ~win32_session();
    virtual session_id_t get_id() const override;
    virtual life_stream  get_life_stream() override;
    virtual message_stream get_cts_stream() override;
    virtual message_stream get_stc_stream() override;
    win32_message_stream get_cts_stream_native();
    win32_message_stream get_stc_stream_native();
    virtual protocol::platform get_platform() const override;
    win32_session();
    win32_session(win32_session&& other);
    win32_session& operator=(win32_session&& other);
protected:
    static std::pair<win32_message_stream/*CTS*/, win32_message_stream/*STC*/> _message_stream_connection(life_stream raw_stream, const char* pipe_name) throw(...);
protected:
    session_id_t _session_id;
    life_stream  _life_stream = nullptr;
    win32_message_stream _cts_stream = nullptr;
    win32_message_stream _stc_stream = nullptr;
};

}

#endif // _OPENNUIG3_KERNEL_SESSION_WIN32_SESSION_H_