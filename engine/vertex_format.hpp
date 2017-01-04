/* Created by Darren Otgaar on 2016/04/02. http://www.github.com/otgaard/zap */
#ifndef ZAP_BUFFER_FORMAT_HPP
#define ZAP_BUFFER_FORMAT_HPP

#include <memory>
#include <cstring>
#include "engine.hpp"
#include <maths/maths.hpp>
#include "vertex_attribute.hpp"

#include <core/pod.hpp>

namespace zap { namespace engine {

namespace detail {
    // Vectors, Matrices, and other wrapped types are expected to conform to this interface
    template <typename T>
    struct attribute_info {
        using type = typename T::type;
        constexpr static size_t size = T::size();
        constexpr static bool is_int_type = attribute_info<type>::is_int_type;
    };

    template <> struct attribute_info<byte> {
        using type = byte;
        constexpr static size_t size = 1;
        constexpr static bool is_int_type = true;
    };

    template <> struct attribute_info<short> {
        using type = short;
        constexpr static size_t size = 1;
        constexpr static bool is_int_type = true;
    };

    template <> struct attribute_info<unsigned short> {
        using type = unsigned short;
        constexpr static size_t size = 1;
        constexpr static bool is_int_type = true;
    };

    template <> struct attribute_info<int> {
        using type = unsigned short;
        constexpr static size_t size = 1;
        constexpr static bool is_int_type = true;
    };

    template <> struct attribute_info<unsigned int> {
        using type = unsigned int;
        constexpr static size_t size = 1;
        constexpr static bool is_int_type = true;
    };

    template <> struct attribute_info<float> {
        using type = float;
        constexpr static size_t size = 1;
        constexpr static bool is_int_type = false;
    };

    template <> struct attribute_info<double> {
        using type = double;
        constexpr static size_t size = 1;
        constexpr static bool is_double_type = true;    // This is meant to generate an error, no support for double types yet
    };

    template <size_t k, typename Vertex>
    struct vattrib_count {
        enum {
            value = attribute_info<typename core::pod_query<k, Vertex>::type::type>::size
        };
    };

    template <size_t k, typename Vertex>
    struct vattrib_is_int {
        enum {
            value = attribute_info<typename core::pod_query<k, Vertex>::type::type>::is_int_type
        };
    };

    template <size_t k, typename Vertex>
    struct vattrib_datatype {
        enum {
            value = dt_descriptor<typename attribute_info<typename core::pod_query<k, Vertex>::type::type>::type>::value
        };
    };
}

template <typename... Args>
struct vertex : core::pod<Args...> {
    using pod_t = core::pod<Args...>;
    using id_t = typename pod_t::id_t;
    constexpr static size_t size = sizeof...(Args);

    using offsets = typename core::generate_table<size, pod_t, core::pod_offset>::result;
    using types = typename core::generate_table<size, pod_t, core::pod_id>::result;
    using counts = typename core::generate_table<size, pod_t, detail::vattrib_count>::result;
    using is_int = typename core::generate_table<size, pod_t, detail::vattrib_is_int>::result;
    using datatypes = typename core::generate_table<size, pod_t, detail::vattrib_datatype>::result;
    using fieldsizes = typename core::generate_table<size, pod_t, core::pod_size>::result;
    constexpr static size_t bytesize() { return sizeof(vertex); }

    static size_t find(id_t id) {
        for(size_t i = 0; i != size; ++i) if((id_t)types::data[i] == id) return i;
        return INVALID_IDX;
    }

    template <typename T> void set(size_t idx, const T& value) {
        assert(idx < size && fieldsizes::data[idx] == sizeof(T) && "Attempt to access invalid field in vertex");
        if(idx < size && fieldsizes::data[idx] == sizeof(T))
            memcpy((((unsigned char*) this) + offsets::data[idx]), &value, sizeof(T));
    }

    vertex() { }
    vertex(const Args&... args) : core::pod<Args...>(args...) { }
};

template <typename VTX_T>
class vertex_iterator : public std::iterator<std::random_access_iterator_tag, VTX_T> {
public:
    using vertex_t = VTX_T;
    vertex_iterator(vertex_t* ptr) : ptr_(ptr) { }
    vertex_iterator(const vertex_iterator& rhs) : ptr_(rhs.ptr_) { }
    vertex_iterator& operator++() { ++ptr_; return *this; }
    vertex_iterator operator++(int) { vertex_iterator tmp(*this); operator++(); return tmp; }
    bool operator==(const vertex_iterator& rhs) { return ptr_ == rhs.ptr_; }
    bool operator!=(const vertex_iterator& rhs) { return ptr_ != rhs.ptr_; }
    vertex_t& operator*() { return *ptr_; }
    vertex_t* operator->() { return ptr_; }
    vertex_iterator& operator--() { --ptr_; return *this; }
    vertex_iterator operator--(int) { vertex_iterator tmp(*this); operator--(); return tmp; }

    template <typename T> friend bool operator<(const vertex_iterator& lhs, const vertex_iterator& rhs);
    template <typename T> friend bool operator>(const vertex_iterator& lhs, const vertex_iterator& rhs);
    template <typename T> friend bool operator<=(const vertex_iterator& lhs, const vertex_iterator& rhs);
    template <typename T> friend bool operator>=(const vertex_iterator& lhs, const vertex_iterator& rhs);

    vertex_iterator& operator+=(size_t v) { ptr_ += v; return *this; }
    template <typename T> friend vertex_iterator<T> operator+(const vertex_iterator<T>& lhs, size_t rhs);
    template <typename T> friend vertex_iterator<T> operator+(size_t lhs, const vertex_iterator<T>& rhs);
    vertex_iterator& operator-=(size_t v) { ptr_ -= v; return *this; }
    template <typename T> friend vertex_iterator<T> operator-(const vertex_iterator<T>& lhs, size_t rhs);
    template <typename T> friend std::ptrdiff_t operator-(const vertex_iterator<T>& lhs, const vertex_iterator<T>& rhs);

private:
    vertex_t* ptr_;
};

template <typename T>
bool operator<(const vertex_iterator<T>& lhs, const vertex_iterator<T>& rhs) {
    return lhs.ptr_ < rhs.ptr_;
}

template <typename T>
bool operator>(const vertex_iterator<T>& lhs, const vertex_iterator<T>& rhs) {
    return lhs.ptr_ > rhs.ptr_;
}

template <typename T>
bool operator<=(const vertex_iterator<T>& lhs, const vertex_iterator<T>& rhs) {
    return lhs.ptr_ <= rhs.ptr_;
}

template <typename T>
bool operator>=(const vertex_iterator<T>& lhs, const vertex_iterator<T>& rhs) {
    return lhs.ptr_ >= rhs.ptr_;
}

template <typename T>
vertex_iterator<T> operator+(const vertex_iterator<T>& lhs, size_t rhs) {
    return vertex_iterator<T>(lhs.ptr_ + rhs);
}

template <typename T>
vertex_iterator<T> operator+(size_t lhs, const vertex_iterator<T>& rhs) {
    return vertex_iterator<T>(rhs.ptr_ + lhs);
}

template <typename T>
vertex_iterator<T> operator-(const vertex_iterator<T>& lhs, size_t rhs) {
    return vertex_iterator<T>(lhs.ptr_ - rhs);
}

template <typename T>
std::ptrdiff_t operator-(const vertex_iterator<T>& lhs, const vertex_iterator<T>& rhs) {
    return lhs.ptr_ - rhs.ptr_;
}

//#pragma pack(pop)

}}

#endif //ZAP_BUFFER_FORMAT_HPP
