/* Created by Darren Otgaar on 2016/04/02. http://www.github.com/otgaard/zap */
#ifndef ZAP_BUFFER_FORMAT_HPP
#define ZAP_BUFFER_FORMAT_HPP

#include "engine.hpp"
#include <maths/maths.hpp>
#include "vertex_attribute.hpp"

namespace zap { namespace engine {

// Alignment of 4 bytes
#pragma pack(push, 4)

struct index_type {
    constexpr static size_t IT_SIZE = 0;
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

template <size_t... P> struct type_table {
    static const size_t data[sizeof...(P)];
};

template <size_t... P> const size_t type_table<P...>::data[sizeof...(P)] = { P... };

template <size_t k, typename POD_T, template <size_t, typename> class FNC, size_t... arr>
struct table_generator_i {
    typedef typename table_generator_i<k-1, POD_T, FNC, FNC<k, POD_T>::value, arr...>::result result;
};

template <typename POD_T, template <size_t, typename> class FNC, size_t... arr>
struct table_generator_i<0, POD_T, FNC, arr...> {
    typedef type_table<FNC<0, POD_T>::value, arr...> result;
};

template <size_t k, typename POD_T, template <size_t, typename>  class FNC>
struct generate_table {
    typedef typename table_generator_i<k-1, POD_T, FNC>::result result;
};

template <size_t k, typename POD_T>
struct vattrib_offset {
    enum {
        value = offset_table<k, POD_T>::offset                  // pod value offset in vertex type
    };
};

template <size_t k, typename POD_T>
struct vattrib_type {
    enum {
        value = engine::typecode<k, POD_T>()                    // attribute_type of vertex attribute
    };
};

template <size_t k, typename POD_T>
struct vattrib_count {
    enum {
        value = type_query<k, POD_T>::type::type::size()        // wrapper type -> stored type -> elements
    };
};

template <size_t k, typename POD_T>
struct vattrib_datatype {
    enum {
        value = dt_descriptor<typename type_query<k, POD_T>::type::type::type>::value
    };
};

template <typename... Args>
struct vertex : pod<Args...> {
    typedef pod<Args...> pod_t;
    constexpr static size_t attrib_count() { return sizeof...(Args); }
    typedef typename generate_table<sizeof...(Args), pod_t, vattrib_offset>::result offsets;
    typedef typename generate_table<sizeof...(Args), pod_t, vattrib_type>::result types;
    typedef typename generate_table<sizeof...(Args), pod_t, vattrib_count>::result counts;
    typedef typename generate_table<sizeof...(Args), pod_t, vattrib_datatype>::result datatypes;
    constexpr static size_t size() { return sizeof(vertex); }
    template <size_t k> constexpr static attribute_type::bitfield typecode() { return engine::typecode<k, pod_t>(); }
    template <size_t k> constexpr static size_t index() { return maths::log2_pow2(uint32_t(engine::typecode<k, pod_t>())); }
    template <size_t k> constexpr static size_t attrib_size() { return sizeof(typename type_query<k, pod_t>::type); }
    template <size_t k> constexpr static size_t attrib_offset() { return engine::offset_table<k, pod_t>::offset; }

    vertex() { }
    vertex(Args... args) : pod<Args...>(args...) { }
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
    template <typename T> friend vertex_iterator operator+(const vertex_iterator& lhs, size_t rhs);
    template <typename T> friend vertex_iterator operator+(size_t lhs, const vertex_iterator& rhs);
    vertex_iterator& operator-=(size_t v) { ptr_ -= v; return *this; }
    template <typename T> friend vertex_iterator<T> operator-(const vertex_iterator<T>& lhs, size_t rhs);
    template <typename T> friend int operator-(const vertex_iterator& lhs, const vertex_iterator& rhs);

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
int operator-(const vertex_iterator<T>& lhs, const vertex_iterator<T>& rhs) {
    return lhs.ptr_ - rhs.ptr_;
}

#pragma pack(pop)

}}

#endif //ZAP_BUFFER_FORMAT_HPP
