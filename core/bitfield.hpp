//
// Created by Darren Otgaar on 2016/05/04.
//

#ifndef ZAP_BITFIELD_HPP
#define ZAP_BITFIELD_HPP

#include "meta.hpp"
#include <maths/maths.hpp>

namespace zap { namespace core {

using zap::maths::pow2;

template <typename T, size_t... Bits>
struct bitfield;

template <size_t k, typename T, size_t... Bits> struct bitfield_query;
template <typename T, size_t First, size_t... Bits> struct bitfield_query<0, bitfield<T, First, Bits...>> {
    enum {
        value = First
    };
};
template <size_t k, typename T, size_t First, size_t... Bits> struct bitfield_query<k, bitfield<T, First, Bits...>> {
    enum {
        value = bitfield_query<k-1, bitfield<T, Bits...>>::value
    };
};

template <size_t k, typename Bitfield> struct bitfield_size;
template <typename Bitfield> struct bitfield_size<0, Bitfield> {
    constexpr static size_t value = bitfield_query<0,Bitfield>::value;
};

template <size_t k, typename Bitfield> struct bitfield_size {
    constexpr static size_t value = bitfield_query<k,Bitfield>::value;
};

template <size_t k, typename Bitfield> struct bitfield_power {
    enum {
        value = pow2<bitfield_query<k,Bitfield>::value>::value - 1
    };
};

template <size_t k, typename Bitfield> struct bitfield_offset;
template <typename Bitfield> struct bitfield_offset<0, Bitfield> {
    constexpr static size_t value = 0;
};

template <size_t k, typename Bitfield> struct bitfield_offset {
    constexpr static size_t value = bitfield_query<k-1,Bitfield>::value + bitfield_offset<k-1,Bitfield>::value;
};

template <size_t k, typename Bitfield> struct bitfield_shift;
template <typename Bitfield> struct bitfield_shift<0, Bitfield> {
    constexpr static size_t value = Bitfield::bitsize - bitfield_query<0,Bitfield>::value;
};

template <size_t k, typename Bitfield> struct bitfield_shift {
    constexpr static size_t value = Bitfield::bitsize -
                                    (bitfield_offset<k,Bitfield>::value + bitfield_query<k,Bitfield>::value);
};

template <size_t k, typename Bitfield> struct bitfield_mask;
template <typename Bitfield> struct bitfield_mask<0, Bitfield> {
    constexpr static size_t value = (pow2<Bitfield::bitsize>::value - 1) ^ (pow2<bitfield_shift<0,Bitfield>::value>::value - 1);
};

template <size_t k, typename Bitfield> struct bitfield_mask {
    constexpr static size_t value = (pow2<bitfield_shift<k-1,Bitfield>::value>::value - 1) ^ (pow2<bitfield_shift<k,Bitfield>::value>::value - 1);
};

template <typename T, size_t... Bits>
struct bitfield {
    using type = T;
    constexpr static size_t bytesize = sizeof(T);
    constexpr static size_t bitsize = bytesize*8;
    constexpr static size_t size = sizeof...(Bits);
    using bits = typename generate_table<size, bitfield<T,Bits...>,bitfield_size>::result;
    using powers = typename generate_table<size, bitfield<T,Bits...>,bitfield_power>::result;
    using offsets = typename generate_table<size, bitfield<T,Bits...>,bitfield_offset>::result;
    using shifts = typename generate_table<size, bitfield<T,Bits...>,bitfield_shift>::result;
    using masks = typename generate_table<size, bitfield<T,Bits...>,bitfield_mask>::result;

    constexpr bitfield() : value(0) { }
    T operator[](size_t id) const { return T(value & T(masks::data[id])) >> shifts::data[id]; }
    T get(size_t id) const { return operator[](id); }
    void set(size_t id, T v) { value = (value & (T(~masks::data[id]))) | ((v & T(powers::data[id])) << shifts::data[id]); }

    T value;
};

template <typename Parm> struct is_bitfield : std::false_type { };
template <typename T, size_t... Bits> struct is_bitfield<bitfield<T, Bits...>> : std::true_type { };

}}

#endif //ZAP_BITFIELD_HPP
