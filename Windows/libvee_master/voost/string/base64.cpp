#include <vee/voost/string.h>
#include <boost/archive/iterators/insert_linebreaks.hpp>
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

string encode(const std::vector<unsigned char>& data)
{
    using namespace boost::archive::iterators;

    typedef
        insert_linebreaks<         // insert line breaks every 72 characters
        base64_from_binary<    // convert binary values ot base64 characters
        transform_width<   // retrieve 6 bit integers from a sequence of 8 bit bytes
        const char *,
        6,
        8
        >
        >
        , 72
        >
        base64_iterator;

    std::vector<unsigned char> buff(data);
    size_t number_of_padd_chars = (buff.size() % 3 != 0) ? 3 - (buff.size() % 3) : 0;
    buff.insert(buff.end(), number_of_padd_chars, '\0');

    base64_iterator begin(&buff[0]), end(&buff[0] + data.size());
    std::string result(begin, end);
    result.insert(result.end(), number_of_padd_chars, '=');

    return result;
}

} // namespace base64
} // namespace voost
} // namespace vee