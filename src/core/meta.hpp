//
// Created by Darren Otgaar on 2016/05/06.
//

#ifndef ZAP_META_HPP_HPP
#define ZAP_META_HPP_HPP

#include <cctype>

// A collection of templates to enable metaprogramming.

namespace zap { namespace core {
/*
 * generate_table<k, Template, template <size_t, Template> Function>
 *
 * Allows executing arbitrary Meta functions at compile time to populate constexpr static tables.  This allows the
 * various pod types and bitfields to be prepared and checked at compile-time rather than run-time.
 *
 */

namespace detail {
    template<size_t... Parms>
    struct data_table {
        static constexpr size_t data[sizeof...(Parms)] = {Parms...};
    };

    template<size_t... Parms> constexpr size_t data_table<Parms...>::data[sizeof...(Parms)];

    template<size_t k, typename Template, template<size_t, typename> class Function, size_t... Parms>
    struct table_gen_i {
        typedef typename table_gen_i<k-1, Template, Function, Function<k, Template>::value, Parms...>::result result;
    };

    template<typename Template, template<size_t, typename> class Function, size_t... Parms>
    struct table_gen_i<0, Template, Function, Parms...> {
        typedef data_table<Function<0, Template>::value, Parms...> result;
    };
}

template <size_t k, typename Template, template <size_t, typename> class Function>
struct generate_table {
    typedef typename detail::table_gen_i<k-1, Template, Function>::result result;
};

}}

#endif //ZAP_META_HPP_HPP
