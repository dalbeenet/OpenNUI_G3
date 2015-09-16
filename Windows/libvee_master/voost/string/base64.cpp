#include <vee/voost/string.h>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/algorithm/string.hpp>

namespace vee {
namespace voost{
namespace base64 {

string decode(const string &dst)
{
    using namespace boost::archive::iterators;
    using It = transform_width<binary_from_base64<std::string::const_iterator>, 8, 6>;
    return boost::algorithm::trim_right_copy_if(std::string(It(std::begin(dst)), It(std::end(dst))), [](char c)
    {
        return c == '\0';
    });
}

string encode(const string &dst)
{
    using namespace boost::archive::iterators;
    using It = base64_from_binary<transform_width<std::string::const_iterator, 6, 8>>;
    auto tmp = std::string(It(std::begin(dst)), It(std::end(dst)));
    return tmp.append((3 - dst.size() % 3) % 3, '=');
}

} // namespace base64
} // namespace voost
} // namespace vee