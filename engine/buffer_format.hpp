/* Created by Darren Otgaar on 2016/04/02. http://www.github.com/otgaard/zap */
#ifndef ZAP_BUFFER_FORMAT_HPP
#define ZAP_BUFFER_FORMAT_HPP

#include "engine.hpp"

namespace zap { namespace engine {

// Alignment of 4 bytes
#pragma pack(push, 4)

struct index_type {
    constexpr static size_t IT_SIZE = 0;
};

template <typename T, attribute_type::bitfield CODE>
struct vertex_attribute {
};

template <typename T> struct vertex_attribute<T, attribute_type::AT_POSITION> {
    typedef T type;
    constexpr static size_t AT_CODE = attribute_type::AT_POSITION;
    constexpr static size_t size() { return sizeof(vertex_attribute); }

    vertex_attribute() { }
    vertex_attribute(const T& v) : value(v) { }
    vertex_attribute(const vertex_attribute& rhs) : value(rhs.value) { }
    vertex_attribute& operator=(const vertex_attribute& rhs) { if(this != &rhs) value = rhs.value; return *this; }

    union {
        T position;
        T value;
    };
};

template <typename T> struct vertex_attribute<T, attribute_type::AT_NORMAL> {
    typedef T type;
    constexpr static size_t AT_CODE = attribute_type::AT_NORMAL;
    constexpr static size_t size() { return sizeof(vertex_attribute); }

    vertex_attribute() { }
    vertex_attribute(const T& v) : value(v) { }
    vertex_attribute(const vertex_attribute& rhs) : value(rhs.value) { }
    vertex_attribute& operator=(const vertex_attribute& rhs) { if(this != &rhs) value = rhs.value; return *this; }

    union {
        T normal;
        T value;
    };
};

template <typename T> struct vertex_attribute<T, attribute_type::AT_TANGENT> {
    typedef T type;
    constexpr static size_t AT_CODE = attribute_type::AT_TANGENT;
    constexpr static size_t size() { return sizeof(vertex_attribute); }

    vertex_attribute() { }
    vertex_attribute(const T& v) : value(v) { }
    vertex_attribute(const vertex_attribute& rhs) : value(rhs.value) { }
    vertex_attribute& operator=(const vertex_attribute& rhs) { if(this != &rhs) value = rhs.value; return *this; }

    union {
        T tangent;
        T value;
    };
};

template <typename T> struct vertex_attribute<T, attribute_type::AT_BINORMAL> {
    typedef T type;
    constexpr static size_t AT_CODE = attribute_type::AT_BINORMAL;
    constexpr static size_t size() { return sizeof(vertex_attribute); }

    vertex_attribute() { }
    vertex_attribute(const T& v) : value(v) { }
    vertex_attribute(const vertex_attribute& rhs) : value(rhs.value) { }
    vertex_attribute& operator=(const vertex_attribute& rhs) { if(this != &rhs) value = rhs.value; return *this; }

    union {
        T binormal;
        T value;
    };
};

template <typename T> struct vertex_attribute<T, attribute_type::AT_TEXCOORD1> {
    typedef T type;
    constexpr static size_t AT_CODE = attribute_type::AT_TEXCOORD1;
    constexpr static size_t size() { return sizeof(vertex_attribute); }

    vertex_attribute() { }
    vertex_attribute(const T& v) : value(v) { }
    vertex_attribute(const vertex_attribute& rhs) : value(rhs.value) { }
    vertex_attribute& operator=(const vertex_attribute& rhs) { if(this != &rhs) value = rhs.value; return *this; }

    union {
        T texcoord1;
        T value;
    };
};

// A variadic POD type
template <typename... Arg>
struct pod {
};

template <typename Arg, typename... Args>
struct pod<Arg, Args...> : Arg, pod<Args...> {
    using typename Arg::type;
    using Arg::value;

    pod() { }
    pod(Arg arg, Args... args) : Arg(arg), pod<Args...>(args...) { }
};

template <size_t k, typename Arg, typename... Args> struct type_query;

template <typename Arg, typename... Args>
struct type_query<0, pod<Arg, Args...>> {
    typedef Arg type;
};

template <size_t k, typename Arg, typename... Args>
struct type_query<k, pod<Arg, Args...>> {
    typedef typename type_query<k-1, pod<Args...>>::type type;
};

template <size_t k, typename... Args>
constexpr typename std::enable_if<k == 0, typename type_query<0, pod<Args...>>::type>::type& get_attrib(pod<Args...>& t) {
    return t;
}

template <size_t k, typename Arg, typename... Args>
constexpr typename std::enable_if<k != 0, typename type_query<k, pod<Arg, Args...>>::type>::type& get_attrib(pod<Arg, Args...>& t) {
    pod<Args...>& base = t;
    return get_attrib<k - 1>(base);
}

template <size_t k, typename... Args>
constexpr typename std::enable_if<k == 0, typename type_query<0, pod<Args...>>::type>::type::type& get(pod<Args...>& t) {
    return t.value;
}

template <size_t k, typename Arg, typename... Args>
constexpr typename std::enable_if<k != 0, typename type_query<k, pod<Arg, Args...>>::type>::type::type& get(pod<Arg, Args...>& t) {
    pod<Args...>& base = t;
    return get<k - 1>(base);
}

template <size_t k, typename T>
constexpr attribute_type::bitfield typecode() {
    return (attribute_type::bitfield)type_query<k,T>::type::AT_CODE;
}

template <size_t k, typename T>
struct offset_table;

template <typename T>
struct offset_table<0, T> {
    constexpr static size_t offset = 0;
};

template <size_t k, typename T>
struct offset_table {
    static_assert(is_specialisation_of<pod, T>::value, "T must be a specialisation of pod<>");
    constexpr static size_t offset = type_query<k-1,T>::type::size() + offset_table<k-1,T>::offset;
};

template <typename... Args>
struct vertex : pod<Args...> {
    typedef pod<Args...> pod_t;
    constexpr static size_t size() { return sizeof...(Args); }
    template <size_t k> constexpr static attribute_type::bitfield typecode() { return engine::typecode<k, pod_t>(); }
    template <size_t k> constexpr static size_t attrib_size() { return sizeof(typename type_query<k, pod_t>::type); }
    template <size_t k> constexpr static size_t attrib_offset() { return engine::offset_table<k, pod_t>::offset; }

    vertex() { }
    vertex(Args... args) : pod<Args...>(args...) { }
};

template <typename... Args>
struct index : pod<Args...> {

};

#pragma pack(pop)

}}

#endif //ZAP_BUFFER_FORMAT_HPP
