//
// Created by Darren Otgaar on 2016/04/30.
//

#ifndef ZAP_PIXEL_FORMAT_HPP
#define ZAP_PIXEL_FORMAT_HPP

#include <maths/maths.hpp>
#include <core/bitfield.hpp>
#include <core/pod.hpp>

namespace zap { namespace engine {
    using zap::maths::pow2;

    enum class channel_type : byte {
        CT_NONE = 0,
        CT_ALPHA = 1,
        CT_LUMINANCE = 2,
        CT_RED = 3,
        CT_GREEN = 4,
        CT_BLUE = 5,
        CT_DEPTH = 6,
        CT_STENCIL = 7
    };

    // Default pixel data is unpacked array
    template <typename T, size_t len, class Enable = void>
    struct pixel_data {
        using type = T;
        constexpr static size_t size = len;

        constexpr pixel_data() = default;
        void set(size_t id, T v) { checkidx(id,size); data[id] = v; }
        T get(size_t id) const { checkidx(id,size); return data[id]; }

        T data[size];
    };

    // Specialisation for bitfield
    template <typename T, size_t len>
    struct pixel_data<T, len, typename std::enable_if<core::is_bitfield<T>::value>::type> {
        using type = typename T::type;
        constexpr static size_t size = len;

        constexpr pixel_data() = default;
        void set(size_t id, type v) { checkidx(id,size); data.set(id, v); }
        type get(size_t id) const { checkidx(id,size); return data.get(id); }

        T data;
    };

    template <channel_type Type, size_t Bits>
    struct channel {
        constexpr static channel_type ch_type = Type;
        constexpr static size_t bits = Bits;
    };

    template <size_t k, typename Channels> struct channel_query {
        constexpr static size_t value = size_t(core::pod_query<k, Channels>::type::ch_type);
    };

    template <size_t k, typename Channels> struct bits_query {
        constexpr static size_t value = size_t(core::pod_query<k, Channels>::type::bits);
    };

    template<typename T, typename... Channels>
    struct pixel : pixel_data<T, sizeof...(Channels)> {
        static_assert(std::is_trivially_copyable<pixel_data<T, sizeof...(Channels)>>::value, "pixel<> must be trivially copyable");
        using data_t = pixel_data<T, sizeof...(Channels)>;
        constexpr static size_t bytesize = sizeof(data_t);
        using channels = core::pod<Channels...>;
        using type = typename data_t::type;
        using channel_types = typename core::generate_table<data_t::size, channels, channel_query>::result;
        using bits = typename core::generate_table<data_t::size, channels, bits_query>::result;
    };

    using red3_t = channel<channel_type::CT_RED, 3>;
    using green3_t = channel<channel_type::CT_GREEN, 3>;
    using blue2_t = channel<channel_type::CT_BLUE, 2>;

    using red8_t = channel<channel_type::CT_RED, 8>;
    using green8_t = channel<channel_type::CT_GREEN, 8>;
    using blue8_t = channel<channel_type::CT_BLUE, 8>;
    using alpha8_t = channel<channel_type::CT_ALPHA, 8>;

    using rgb332_t = pixel<core::bitfield<byte, 3, 3, 2>, red3_t, green3_t, blue2_t>;
    using rgb888_t = pixel<byte, red8_t, green8_t, blue8_t>;
    using rgba8888_t = pixel<byte, red8_t, green8_t, blue8_t, alpha8_t>;
}}

#endif //ZAP_PIXEL_FORMAT_HPP
