#ifndef _VEE_TYPE_PARSER_H_
#define _VEE_TYPE_PARSER_H_

#include <tuple>
#include <functional>

namespace vee {

    template <class FTy>
    struct function_type_parser
    {
    };

    template <class RTy, typename ...Args>
    struct function_type_parser < RTy(Args ...) >
    {
        typedef RTy return_type;
        typedef std::tuple<Args ...> argspack_type;
        template <int INDEX>
        struct argtype
        {
            template <std::size_t N>
            using type = typename std::tuple_element<N, std::tuple<Args...>>::type;
        };
    };

    template <class FuncSig> // parse std::function type
    struct function_type_parser< std::function<FuncSig> > : function_type_parser<FuncSig>
    {
    };
}

#endif // _VEE_TYPE_PARSER_H_