//
// Created by Darren Otgaar on 2016/04/09.
//

#ifndef ZAP_VERTEX_ATTRIBUTE_HPP
#define ZAP_VERTEX_ATTRIBUTE_HPP

#include "engine.hpp"
#include <maths/maths.hpp>

namespace zap { namespace engine {

#pragma pack(push, 4)

template <typename T, attribute_type::bitfield CODE>
struct vertex_attribute {
};

template <typename T> struct vertex_attribute<T, attribute_type::AT_POSITION> {
    using type = T;
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
    using type = T;
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
    using type = T;
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
    using type = T;
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
    using type = T;
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

template <typename T> struct vertex_attribute<T, attribute_type::AT_TEXCOORD2> {
    using type = T;
    constexpr static size_t AT_CODE = attribute_type::AT_TEXCOORD2;
    constexpr static size_t size() { return sizeof(vertex_attribute); }

    vertex_attribute() { }
    vertex_attribute(const T& v) : value(v) { }
    vertex_attribute(const vertex_attribute& rhs) : value(rhs.value) { }
    vertex_attribute& operator=(const vertex_attribute& rhs) { if(this != &rhs) value = rhs.value; return *this; }
    union {
        T texcoord2;
        T value;
    };
};

template <typename T> struct vertex_attribute<T, attribute_type::AT_TEXCOORD3> {
    using type = T;
    constexpr static size_t AT_CODE = attribute_type::AT_TEXCOORD3;
    constexpr static size_t size() { return sizeof(vertex_attribute); }

    vertex_attribute() { }
    vertex_attribute(const T& v) : value(v) { }
    vertex_attribute(const vertex_attribute& rhs) : value(rhs.value) { }
    vertex_attribute& operator=(const vertex_attribute& rhs) { if(this != &rhs) value = rhs.value; return *this; }
    union {
        T texcoord3;
        T value;
    };
};

template <typename T> struct vertex_attribute<T, attribute_type::AT_TEXCOORD4> {
    using type = T;
    constexpr static size_t AT_CODE = attribute_type::AT_TEXCOORD4;
    constexpr static size_t size() { return sizeof(vertex_attribute); }

    vertex_attribute() { }
    vertex_attribute(const T& v) : value(v) { }
    vertex_attribute(const vertex_attribute& rhs) : value(rhs.value) { }
    vertex_attribute& operator=(const vertex_attribute& rhs) { if(this != &rhs) value = rhs.value; return *this; }
    union {
        T texcoord4;
        T value;
    };
};

template <typename T> struct vertex_attribute<T, attribute_type::AT_TEXCOORD5> {
    using type = T;
    constexpr static size_t AT_CODE = attribute_type::AT_TEXCOORD5;
    constexpr static size_t size() { return sizeof(vertex_attribute); }

    vertex_attribute() { }
    vertex_attribute(const T& v) : value(v) { }
    vertex_attribute(const vertex_attribute& rhs) : value(rhs.value) { }
    vertex_attribute& operator=(const vertex_attribute& rhs) { if(this != &rhs) value = rhs.value; return *this; }
    union {
        T texcoord5;
        T value;
    };
};

template <typename T> struct vertex_attribute<T, attribute_type::AT_TEXCOORD6> {
    using type = T;
    constexpr static size_t AT_CODE = attribute_type::AT_TEXCOORD6;
    constexpr static size_t size() { return sizeof(vertex_attribute); }

    vertex_attribute() { }
    vertex_attribute(const T& v) : value(v) { }
    vertex_attribute(const vertex_attribute& rhs) : value(rhs.value) { }
    vertex_attribute& operator=(const vertex_attribute& rhs) { if(this != &rhs) value = rhs.value; return *this; }
    union {
        T texcoord6;
        T value;
    };
};

template <typename T> struct vertex_attribute<T, attribute_type::AT_COLOUR1> {
    using type = T;
    constexpr static size_t AT_CODE = attribute_type::AT_COLOUR1;
    constexpr static size_t size() { return sizeof(vertex_attribute); }

    vertex_attribute() { }
    vertex_attribute(const T& v) : value(v) { }
    vertex_attribute(const vertex_attribute& rhs) : value(rhs.value) { }
    vertex_attribute& operator=(const vertex_attribute& rhs) { if(this != &rhs) value = rhs.value; return *this; }
    union {
        T colour1;
        T value;
    };
};

template <typename T> struct vertex_attribute<T, attribute_type::AT_COLOUR2> {
    using type = T;
    constexpr static size_t AT_CODE = attribute_type::AT_COLOUR2;
    constexpr static size_t size() { return sizeof(vertex_attribute); }

    vertex_attribute() { }
    vertex_attribute(const T& v) : value(v) { }
    vertex_attribute(const vertex_attribute& rhs) : value(rhs.value) { }
    vertex_attribute& operator=(const vertex_attribute& rhs) { if(this != &rhs) value = rhs.value; return *this; }
    union {
        T colour2;
        T value;
    };
};

template <typename T> struct vertex_attribute<T, attribute_type::AT_BLENDINDEX> {
    using type = T;
    constexpr static size_t AT_CODE = attribute_type::AT_BLENDINDEX;
    constexpr static size_t size() { return sizeof(vertex_attribute); }

    vertex_attribute() { }
    vertex_attribute(const T& v) : value(v) { }
    vertex_attribute(const vertex_attribute& rhs) : value(rhs.value) { }
    vertex_attribute& operator=(const vertex_attribute& rhs) { if(this != &rhs) value = rhs.value; return *this; }
    union {
        T blend_index;
        T value;
    };
};

template <typename T> struct vertex_attribute<T, attribute_type::AT_BLENDWEIGHT> {
    using type = T;
    constexpr static size_t AT_CODE = attribute_type::AT_BLENDWEIGHT;
    constexpr static size_t size() { return sizeof(vertex_attribute); }

    vertex_attribute() { }
    vertex_attribute(const T& v) : value(v) { }
    vertex_attribute(const vertex_attribute& rhs) : value(rhs.value) { }
    vertex_attribute& operator=(const vertex_attribute& rhs) { if(this != &rhs) value = rhs.value; return *this; }
    union {
        T blend_weight;
        T value;
    };
};

template <typename T> struct vertex_attribute<T, attribute_type::AT_FOGCOORD> {
    using type = T;
    constexpr static size_t AT_CODE = attribute_type::AT_FOGCOORD;
    constexpr static size_t size() { return sizeof(vertex_attribute); }

    vertex_attribute() { }
    vertex_attribute(const T& v) : value(v) { }
    vertex_attribute(const vertex_attribute& rhs) : value(rhs.value) { }
    vertex_attribute& operator=(const vertex_attribute& rhs) { if(this != &rhs) value = rhs.value; return *this; }
    union {
        T fog_coord;
        T value;
    };
};

template <typename T> struct vertex_attribute<T, attribute_type::AT_POINTSIZE> {
    using type = T;
    constexpr static size_t AT_CODE = attribute_type::AT_POINTSIZE;
    constexpr static size_t size() { return sizeof(vertex_attribute); }

    vertex_attribute() { }
    vertex_attribute(const T& v) : value(v) { }
    vertex_attribute(const vertex_attribute& rhs) : value(rhs.value) { }
    vertex_attribute& operator=(const vertex_attribute& rhs) { if(this != &rhs) value = rhs.value; return *this; }
    union {
        T point_size;
        T value;
    };
};

#pragma pack(pop)

}}

#endif //ZAP_VERTEX_ATTRIBUTE_HPP
