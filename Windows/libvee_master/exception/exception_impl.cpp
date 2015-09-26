#include <vee/exception.h>

namespace vee {

exception::exception(const char* _desc, int _code):
desc(_desc),
code(_code)
{

}

exception::exception(string& _desc, int _code):
desc(_desc),
code(_code)
{
}

exception::exception(string&& _desc, int _code):
desc(static_cast<string&&>(_desc)),
code(_code)
{
}

}