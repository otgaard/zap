//
// Created by Darren Otgaar on 2016/05/01.
//

#ifndef ZAP_POD_HPP
#define ZAP_POD_HPP

#include "meta.hpp"

namespace zap { namespace core {

/*
 * podfield represents an enumerated field.
 * T    - The stored type
 * ID_T - The enumeration type
 * ID   - The enumerator value for the instance
 *
 * Use MAKE_PODFIELD() to generate member names
 */


template <typename T, typename ID_T, ID_T ID>
struct podfield {
    /* This is an issue due to aligned vector/matrix types - how to check for aligned-trivially-copyable?
    static_assert(std::is_trivially_copyable<T>::value, "podfield<> only accepts trivially copyable types");
    */
    using type = T;
    using id_t = ID_T;
    constexpr static id_t field_id = ID;
    podfield() { };
    podfield(const T& v) : value(v) { }
    podfield(const podfield& r) : value(r.value) { }
    podfield& operator=(const podfield& r) { if(this != &r) { value = r.value; } return *this; }
    T value;
};

#define MAKE_PODFIELD(MemberName, ID_T, ID) \
template <typename T> struct podfield<T, ID_T, ID> { \
    /*static_assert(std::is_trivially_copyable<T>::value, "podfield<> only accepts trivially copyable types");*/ \
    using type = T; \
    using id_t = ID_T; \
    constexpr static id_t field_id = ID; \
    podfield() { }; \
    podfield(const T& v) : value(v) { } \
    podfield(const podfield& r) : value(r.value) { } \
    podfield& operator=(const podfield& r) { if(this != &r) { value = r.value; } return *this; } \
    union { \
        T MemberName; \
        T value; \
    }; \
}; \
template <typename T> \
using MemberName = podfield<T, ID_T, ID>;

/* The POD class.  This is a variadic structure that is trivially copyable. */

template <typename... Args>
struct pod {
};

template <typename Parm> struct is_podfield : std::false_type { };
template <typename T, typename ID_T, ID_T ID> struct is_podfield<podfield<T, ID_T, ID>> : std::true_type { };

template <typename Arg, typename... Args>
struct pod<Arg, Args...> : Arg, pod<Args...> {
    using id_t = typename Arg::id_t;
    static_assert(is_podfield<Arg>::value, "pod<> only accepts podfield<> parameters");
    pod() = default;
    pod(const Arg& arg, const Args&... args) : Arg(arg), pod<Args...>(args...) { }
};

/* POD queries (using the generate_table metafunction) */

template <size_t k, typename... Args> struct pod_query;

template <typename Arg, typename... Args>
struct pod_query<0, pod<Arg, Args...>> {
    using type = Arg;
    constexpr static size_t bytesize = sizeof(Arg);
};

template <size_t k, typename Arg, typename... Args>
struct pod_query<k, pod<Arg, Args...>> {
    using type = typename pod_query<k-1, pod<Args...>>::type;
    constexpr static size_t bytesize = pod_query<k-1, pod<Args...>>::bytesize;
};

template <size_t k, typename... Args>
constexpr typename std::enable_if<k==0, typename pod_query<0, pod<Args...>>::type>::type& get(pod<Args...>& D) {
    return D;
};

template <size_t k, typename Arg, typename... Args>
constexpr typename std::enable_if<k!=0, typename pod_query<k, pod<Arg, Args...>>::type>::type& get(pod<Arg, Args...>& D) {
    pod<Args...>& base = D;
    return get<k-1>(base);
};

template <size_t k, typename POD_T> struct pod_offset;
template <typename POD_T> struct pod_offset<0,POD_T> {
    constexpr static size_t value = 0;
};

template <size_t k, typename POD_T> struct pod_offset {
    constexpr static size_t value = pod_query<k-1,POD_T>::bytesize + pod_offset<k-1,POD_T>::value;
};

template <size_t k, typename POD_T> struct pod_id;
template <typename POD_T> struct pod_id<0,POD_T> {
    enum {
        value = size_t(pod_query<0,POD_T>::type::field_id)
    };
};
template <size_t k, typename POD_T> struct pod_id {
    enum {
        value = size_t(pod_query<k,POD_T>::type::field_id)
    };
};

template <size_t k, typename POD_T>
struct pod_size {
    enum {
        value = pod_query<k,POD_T>::bytesize
    };
};

template <typename... Args>
struct mapping : pod<Args...> {
#ifndef __GNUG__
    static_assert(std::is_trivially_copyable<pod<Args...>>::value, "mapping<> must be trivially copyable");
#endif
    using pod_t = pod<Args...>;
    constexpr static size_t size = sizeof...(Args);
    constexpr static size_t bytesize = sizeof(pod_t);

    mapping() = default;
    mapping(const Args&... args) : pod<Args...>(args...) { }
};

}}

#endif //ZAP_POD_HPP
