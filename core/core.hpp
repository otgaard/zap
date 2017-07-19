//
// Created by otgaard on 2016/03/08.
//

#ifndef ZAP_CORE_HPP
#define ZAP_CORE_HPP

#include <memory>
#include <cstdint>
#include <cassert>
#include <type_traits>

// Some missing defs for GCC
#ifdef __GNUG__
#ifdef __linux__
namespace std {
    template<typename T, typename... Args>
    std::unique_ptr<T> make_unique(Args&&... args) {
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }
};
#endif
#endif

constexpr std::uint32_t INVALID_IDX = std::uint32_t(-1);
using uchar = unsigned char;
using byte = unsigned char;
using ushort = unsigned short;
using uint = unsigned int;
using ulong = unsigned long;
using ulonglong = unsigned long long;

#define ZERR_IDX_OUT_OF_RANGE "Index out of range"
#define ZERR_ARG_COUNT_INVALID "Incorrect number of values provided"
#define ZERR_TYPE_INVALID "Invalid type used for template instantiation"
#define ZERR_TYPE_FLOATING "Requires floating-point type"
#define ZERR_UNALLOCATED_BUFFER "buffer is not allocated"

template <typename T>
inline void checkidx(T var, T val) { assert((var) < (val) && ZERR_IDX_OUT_OF_RANGE); }

#define UNUSED(expr) do { (void)(expr); } while (0)

template <template <typename...> class TYPE, typename T>
struct is_specialisation_of : std::false_type { };

template <template <typename...> class TYPE, typename... P>
struct is_specialisation_of<TYPE, TYPE<P...>> : std::true_type { };

/*
 * Some necessary functions I repeatedly need
 */

template <typename T, size_t size, typename FNC>
void shift_array(T* begin, size_t shift, FNC set_fnc) {
    for(size_t i = size-1, end = shift-1; i != end; --i) set_fnc(begin[i], begin[i-shift]);
}

namespace zap {

namespace details {

template <typename Fnc, typename Arg, typename... Args> struct expander {
    static void expand(Fnc fnc, Arg&& arg, Args&&... args) {
        fnc(std::forward<Arg>(arg));
        expander<Fnc, Args...>::expand(fnc, args...);
    }
};

template <typename Fnc, typename Arg> struct expander<Fnc, Arg> {
    static void expand(Fnc fnc, Arg&& arg) {
        fnc(std::forward<Arg>(arg));
    }
};

template<typename Fnc, typename Tuple, size_t... Idx>
auto applier(Fnc&& fnc, Tuple&& tuple, std::index_sequence<Idx...>) {
    return std::forward<Fnc>(fnc)(std::get<Idx>(std::forward<Tuple>(tuple))...);
};

}

template <typename Fnc, typename... Args>
void expand(Fnc fnc, Args&&... args) {
    details::expander<Fnc, Args...>::expand(fnc, std::forward<Args>(args)...);
};

template <typename Fnc, typename Tuple>
auto apply(Fnc&& fnc, Tuple&& tuple) {
    using indices = std::make_index_sequence<std::tuple_size<std::decay_t<Tuple>>::value>;
    return details::applier(std::forward<Fnc>(fnc), std::forward<Tuple>(tuple), indices());
};

}

#endif //ZAP_CORE_HPP
