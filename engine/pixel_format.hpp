//
// Created by Darren Otgaar on 2016/04/30.
//

#ifndef ZAP_PIXEL_FORMAT_HPP
#define ZAP_PIXEL_FORMAT_HPP

#include <maths/maths.hpp>
#include <core/bitfield.hpp>
#include <core/pod.hpp>
#include <maths/vec2.hpp>
#include <maths/vec3.hpp>
#include <maths/vec4.hpp>

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

        pixel_data() = default;
        void set(size_t id, T v) { checkidx(id,size); data[id] = v; }
        T get(size_t id) const { checkidx(id,size); return data[id]; }

        T data[size];
    };

    // Specialisation for bitfield
    template <typename T, size_t len>
    struct pixel_data<T, len, typename std::enable_if<core::is_bitfield<T>::value>::type> {
        using type = typename T::type;
        constexpr static size_t size = len;

        pixel_data() = default;
        void set(size_t id, type v) { checkidx(id,size); data.set(id, v); }
        type get(size_t id) const { checkidx(id,size); return data.get(id); }

        T data;
    };

    template <channel_type Type, size_t Bits>
    struct channel {
        using id_t = channel_type;
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
        constexpr static size_t size = sizeof...(Channels);
        constexpr static size_t bytesize = sizeof(data_t);
        using channels = core::pod<Channels...>;
        using type = typename data_t::type;
        using channel_types = typename core::generate_table<data_t::size, channels, channel_query>::result;
        using bits = typename core::generate_table<data_t::size, channels, bits_query>::result;

        pixel() = default;

        using data_t::set;
        using data_t::get;

        void set1(const T& a) { set(0, a); }
        void set2(const T& a, const T& b) { set(0, a); set(1, b); }
        void set3(const T& a, const T& b, const T& c) { set(0, a); set(1, b), set(2, c); }
        void set4(const T& a, const T& b, const T& c, const T& d) { set(0, a), set(1, b), set(2, c), set(3, d); }

        template <typename S> void set2(const maths::vec2<S>& v) { set2(v[0], v[1]); }
        template <typename S> void set3(const maths::vec3<S>& v) { set3(v[0], v[1], v[2]); }
        template <typename S> void set4(const maths::vec4<S>& v) { set4(v[0], v[1], v[2], v[3]); }

        T get1() const { return get(0); }
        std::tuple<T, T> get2() const { return std::make_tuple(get(0), get(1)); }
        std::tuple<T, T, T> get3() const { return std::make_tuple(get(0), get(1), get(2)); }
        std::tuple<T, T, T, T> get4() const { return std::make_tuple(get(0), get(1), get(2), get(3)); }

        template <typename S> maths::vec2<S> get2v() const { return maths::vec2<S>(get(0), get(1)); }
        template <typename S> maths::vec3<S> get3v() const { return maths::vec3<S>(get(0), get(1), get(2)); }
        template <typename S> maths::vec4<S> get4v() const { return maths::vec4<S>(get(0), get(1), get(2), get(3)); }
    };

    using red3_t = channel<channel_type::CT_RED, 3>;
    using green3_t = channel<channel_type::CT_GREEN, 3>;
    using blue2_t = channel<channel_type::CT_BLUE, 2>;

    using red8_t = channel<channel_type::CT_RED, 8>;
    using green8_t = channel<channel_type::CT_GREEN, 8>;
    using blue8_t = channel<channel_type::CT_BLUE, 8>;
    using alpha8_t = channel<channel_type::CT_ALPHA, 8>;

    using depth24_t = channel<channel_type::CT_DEPTH, 24>;
    using stencil8_t = channel<channel_type::CT_STENCIL, 8>;

    using rgb332_t = pixel<core::bitfield<byte, 3, 3, 2>, red3_t, green3_t, blue2_t>;
    using rgb888_t = pixel<byte, red8_t, green8_t, blue8_t>;
    using rgba8888_t = pixel<byte, red8_t, green8_t, blue8_t, alpha8_t>;
    using d24s8_t = pixel<core::bitfield<uint32_t, 24, 8>, depth24_t, stencil8_t>;

    template <typename PixelT> struct pixel_type { };

#define DEF_PIXEL_TYPE(pix_type, pix_format_id, data_type_id) template <> struct pixel_type<pix_type> { \
        constexpr static pixel_format format = pix_format_id; \
        constexpr static pixel_datatype datatype = data_type_id; \
    };

    DEF_PIXEL_TYPE(rgb332_t, pixel_format::PF_RGB, pixel_datatype::PD_UNSIGNED_BYTE_3_3_2);
    DEF_PIXEL_TYPE(rgb888_t, pixel_format::PF_RGB, pixel_datatype::PD_UNSIGNED_BYTE);
    DEF_PIXEL_TYPE(rgba8888_t, pixel_format::PF_RGBA, pixel_datatype::PD_UNSIGNED_BYTE);
    DEF_PIXEL_TYPE(d24s8_t, pixel_format::PF_DEPTH_STENCIL, pixel_datatype::PD_UNSIGNED_INT_24_8);
}}

#endif //ZAP_PIXEL_FORMAT_HPP
