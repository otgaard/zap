/* Created by Darren Otgaar on 2016/04/02. http://www.github.com/otgaard/zap */
#ifndef ZAP_BUFFER_FORMAT_HPP
#define ZAP_BUFFER_FORMAT_HPP

#include "engine.hpp"

namespace zap { namespace engine {

// Alignment of 4 bytes
#pragma pack(push, 4)

struct attribute_type {
    constexpr static size_t AT_SIZE = 16;
    enum code {
        AT_POSITION = 1 << 0,
        AT_NORMAL = 1 << 1,
        AT_TANGENT = 1 << 2,
        AT_BINORMAL = 1 << 3,
        AT_TEXCOORD1 = 1 << 4,
        AT_TEXCOORD2 = 1 << 5,
        AT_TEXCOORD3 = 1 << 6,
        AT_TEXCOORD4 = 1 << 7,
        AT_TEXCOORD5 = 1 << 8,
        AT_TEXCOORD6 = 1 << 9,
        AT_COLOUR1 = 1 << 10,
        AT_COLOUR2 = 1 << 11,
        AT_BLENDINDEX = 1 << 12,
        AT_BLENDWEIGHT = 1 << 13,
        AT_FOGCOORD = 1 << 14,
        AT_POINTSIZE = 1 << 15
    };
};

struct index_type {
    constexpr static size_t IT_SIZE = 0;
};

template <typename T, attribute_type::code CODE>
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

template <typename... Arg>
struct attribute {
};

template <typename Arg, typename... Args>
struct attribute<Arg, Args...> : Arg, attribute<Args...> {
    using typename Arg::type;
    using Arg::value;

    attribute() { }
    attribute(Arg arg, Args... args) : Arg(arg), attribute<Args...>(args...) { }
};

template <size_t k, typename Arg, typename... Args> struct type_query;

template <typename Arg, typename... Args>
struct type_query<0, attribute<Arg, Args...>> {
    typedef Arg type;
};

template <size_t k, typename Arg, typename... Args>
struct type_query<k, attribute<Arg, Args...>> {
    typedef typename type_query<k-1, attribute<Args...>>::type type;
};

template <size_t k, typename... Args>
constexpr typename std::enable_if<k == 0, typename type_query<0, attribute<Args...>>::type>::type& get_attrib(attribute<Args...>& t) {
    return t;
}

template <size_t k, typename Arg, typename... Args>
constexpr typename std::enable_if<k != 0, typename type_query<k, attribute<Arg, Args...>>::type>::type& get_attrib(attribute<Arg, Args...>& t) {
    attribute<Args...>& base = t;
    return get_attrib<k - 1>(base);
}

template <size_t k, typename... Args>
constexpr typename std::enable_if<k == 0, typename type_query<0, attribute<Args...>>::type>::type::type& get(attribute<Args...>& t) {
    return t.value;
}

template <size_t k, typename Arg, typename... Args>
constexpr typename std::enable_if<k != 0, typename type_query<k, attribute<Arg, Args...>>::type>::type::type& get(attribute<Arg, Args...>& t) {
    attribute<Args...>& base = t;
    return get<k - 1>(base);
}

template <size_t k, typename T>
constexpr attribute_type::code typecode() {
    return (attribute_type::code)type_query<k,T>::type::AT_CODE;
}

template <size_t k, typename T>
struct offset_table;

template <typename T>
struct offset_table<0, T> {
    constexpr static size_t offset = 0;
};

template <size_t k, typename T>
struct offset_table {
    constexpr static size_t offset = type_query<k-1,T>::type::size() + offset_table<k-1,T>::offset;
};

template <typename... Args>
struct vertex : attribute<Args...> {
    typedef attribute<Args...> type;
    constexpr static size_t size() { return sizeof...(Args); }
    template <size_t k> constexpr static attribute_type::code typecode() { return engine::typecode<k, type>(); }
    template <size_t k> constexpr static size_t attrib_size() { return sizeof(typename type_query<k, type>::type); }
    template <size_t k> constexpr static size_t attrib_offset() { return engine::offset_table<k, type>::offset; }

    vertex() { }
    vertex(Args... args) : attribute<Args...>(args...) { }
};

template <typename... Args>
struct index : attribute<Args...> {

};

#pragma pack(pop)

}}

#endif //ZAP_BUFFER_FORMAT_HPP
