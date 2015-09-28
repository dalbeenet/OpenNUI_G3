#ifndef _OPENNUIG3_ERROR_H_
#define _OPENNUIG3_ERROR_H_

namespace kernel {

enum class error_code: int
{
    handshake_failure = 0,
    load_dll_failure,
    could_not_call_module_entry_point,
    remove_module_failure,
};

}

#endif // !_OPENNUIG3_ERROR_H_
