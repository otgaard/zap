//
// Created by Darren Otgaar on 2016/07/03.
//

#ifndef ZAP_VECTOR_HPP
#define ZAP_VECTOR_HPP

#include <maths/maths.hpp>

namespace zap { namespace maths {

template <typename T, size_t N>
struct vector {
    using type = T;
    constexpr static size_t size = N;
    constexpr static size_t bytesize = sizeof(type)*size;

    vector() = default;
    explicit vector(const T& init) { for(int i = 0; i != size; ++i) arr[i] = init; }
    vector(const static_list<T, size>& lst) { for(int i = 0; i != size; ++i) arr[i] = lst[i]; }
    vector(const vector& rhs) { for(int i = 0; i != size; ++i) arr[i] = rhs[i]; }

    const T& operator[](size_t idx) const { checkidx(idx,size); return arr[idx]; }
    T& operator[](size_t idx) { checkidx(idx,size); return arr[idx]; }
    const T* data() const { return &arr[0]; }
    T* data() { return &arr[0]; }

    vector operator=(const vector& rhs) { if(this != &rhs) for(int i = 0; i != size; ++i) arr[i] = rhs[i]; return *this; }

    bool operator==(const vector& rhs) const { return memcmp(&arr[0], rhs.data(), bytesize) == 0; }
    bool operator!=(const vector& rhs) const { return !operator==(rhs); }
    bool operator<(const vector& rhs) const { return memcmp(&arr[0], rhs.data(), bytesize) < 0; }
    bool operator<=(const vector& rhs) const { return memcmp(&arr[0], rhs.data(), bytesize) <= 0; }
    bool operator>(const vector& rhs) const { return memcmp(&arr[0], rhs.data(), bytesize) > 0; }
    bool operator>=(const vector& rhs) const { return memcmp(&arr[0], rhs.data(), bytesize) >= 0; }

    vector operator-() const { vector r; for(int i = 0; i != size; ++i) r[i] = -arr[i]; return r; }
    vector& operator+=(const vector& rhs) { for(int i = 0; i != size; ++i) arr[i] += rhs[i]; return *this; }
    vector& operator-=(const vector& rhs) { for(int i = 0; i != size; ++i) arr[i] -= rhs[i]; return *this; }
    template <typename S> vector& operator*=(const S& scalar) { for(int i = 0; i != size; ++i) arr[i] *= scalar; return *this; }
    template <typename S> vector& operator/=(const S& scalar) { const auto inv_s = S(1)/scalar; return operator*=(inv_s); }

    T arr[size];
};

}}

#endif //ZAP_VECTOR_HPP
