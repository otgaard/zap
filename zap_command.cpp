//
// Created by Darren Otgaar on 2016/03/20.
//

#include <maths/vec3.hpp>
#include <chrono>
#include <vector>
#include <array>

#define LOGGING_ENABLED
#include <tools/log.hpp>

volatile int sink = 0;
using zap_clock = std::chrono::high_resolution_clock;
using vec_t = std::array<int,10>;
constexpr size_t test_idx[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

void test_indexing() {
    std::vector<vec_t> arr(10000000, vec_t({{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}}));

    auto it_start = zap_clock::now();
    for(size_t i = 0; i != arr.size(); ++i) {
        for(size_t j = 0; j != arr[i].size(); ++j) {
            sink += arr[i][j];
        }
    }
    auto it_end = zap_clock::now();

    auto idx_start = zap_clock::now();
    for(size_t i = 0; i != arr.size(); ++i) {
        for(auto j : test_idx) sink += arr[i][j];
    }
    auto idx_end = zap_clock::now();

    std::chrono::duration<double> it_diff = it_end - it_start;
    std::chrono::duration<double> idx_diff = idx_end - idx_start;

    LOG("Time for increment loop", it_diff.count());
    LOG("Time for indexed loop", idx_diff.count());
}
/*
template <typename T>
constexpr T adder(T v) {
    return v;
}

template <typename T, typename... Args>
constexpr T adder(T first, Args... args) {
    LOG(__PRETTY_FUNCTION__, __FILE__, __FUNCTION__, __LINE__);
    return first + adder(args...);
};

void test() {
    LOG(adder(10, 20, 30, 40, 50));
}

template <size_t k, typename T, typename... P>
struct offset_table;

template <typename... P>
struct offset_table<0, P...> {
    constexpr static size_t offset = 0;
};

template <size_t k, typename T, typename... P>
struct offset_table {
    constexpr static size_t offset = sizeof(T) + offset_table<k-1,P...>::offset;
};

void test_offset() {
    LOG("value=", offset_table<3, int, float, char, double>::offset);
}
*/
// These tests determine whether there are performance issues with variadic templates vs hand-written structures.

#include <maths/functions.hpp>
#include <engine/buffer_format.hpp>

struct vstruct {
    zap::maths::vec3f position;
    zap::maths::vec3f normal;
    zap::maths::vec3f tangent;
    zap::maths::vec3f binormal;
};

void test_variadic() {
    using namespace zap::maths;
    using namespace zap::engine;

    timer t;

    constexpr size_t buffer_size = 1000000;

    using pos3_t = vertex_attribute<vec3f, attribute_type::AT_POSITION>;
    using nor3_t = vertex_attribute<vec3f, attribute_type::AT_NORMAL>;
    using tan3_t = vertex_attribute<vec3f, attribute_type::AT_TANGENT>;
    using bin3_t = vertex_attribute<vec3f, attribute_type::AT_BINORMAL>;
    using vertex_t = vertex<pos3_t, nor3_t, tan3_t, bin3_t>;

    t.start();
    double t0 = t.get_time<double>();
    std::vector<vstruct> vstruct_buffer(buffer_size);
    double t1 = t.get_time<double>();
    std::vector<vertex_t> variadic_buffer(buffer_size);
    double t2 = t.get_time<double>();

    LOG("plain", t1 - t0);
    LOG("variadic", t2 - t1);

    for(int i = 0; i < 2000; ++i) {
        auto d = i/1000.f;
        for(auto& v : variadic_buffer) {
            v.position = vec3f(d,d,d);
            v.normal = vec3f(d,d,d);
            v.tangent = vec3f(d,d,d);
            v.binormal = vec3f(d,d,d);
        }
    }

    for(int i = 0; i < 2000; ++i) {
        auto d = i/1000.f;
        for(auto& v : vstruct_buffer) {
            v.position = vec3f(d,d,d);
            v.normal = vec3f(d,d,d);
            v.tangent = vec3f(d,d,d);
            v.binormal = vec3f(d,d,d);
        }
    }

    LOG("\n", "Setting");
    t0 = t.get_time<double>();
    for(int i = 0; i < 10000; ++i) {
        auto d = i/10000.f;
        for(auto& v : vstruct_buffer) {
            v.position = vec3f(d,d,d);
            v.normal = vec3f(d,d,d);
            v.tangent = vec3f(d,d,d);
            v.binormal = vec3f(d,d,d);
        }
    }
    t1 = t.get_time<double>();
    LOG("plain", t1 - t0);
    for(int i = 0; i < 10000; ++i) {
        auto d = i/10000.f;
        for(auto& v : variadic_buffer) {
            v.position = vec3f(d,d,d);
            v.normal = vec3f(d,d,d);
            v.tangent = vec3f(d,d,d);
            v.binormal = vec3f(d,d,d);
        }
    }

    t2 = t.get_time<double>();
    LOG("variadic", t2 - t1);
}

template <typename FNC>
struct wave {
    constexpr static auto make_fnc(FNC fnc, float frequency, float amplitude, float phase) {
        return [=](float x) -> float {
            return amplitude*fnc(x*frequency + phase);
        };
    };
};

void test_wave() {
    auto fnc = wave<decltype(sinf)>::make_fnc(sinf, 1.f, 1.f, 0.f);
    float inc = 1/100.f;
    for(size_t i = 0; i != 100; ++i) {
        LOG(fnc(i*inc));
    }
}

#include <engine/pixel_format.hpp>

using namespace zap;
using namespace zap::core;
using namespace zap::engine;

int main(int argc, char* argv[]) {

    using red3_t = channel<channel_type::CT_RED, 3>;
    using green3_t = channel<channel_type::CT_GREEN, 3>;
    using blue2_t = channel<channel_type::CT_BLUE, 2>;

    pixel<bitfield<byte, 3, 3, 2>, red3_t, green3_t, blue2_t> A;

    A.set(0, 1);
    LOG(size_t(A.get(0)));

    using red8_t = channel<channel_type::CT_RED, 8>;
    using green8_t = channel<channel_type::CT_GREEN, 8>;
    using blue8_t = channel<channel_type::CT_BLUE, 8>;

    pixel<byte, red8_t, green8_t, blue8_t> B;
    B.set(0,1);

    LOG(rgb888_t::channel_types::data[0], rgb888_t::channel_types::data[1], rgb888_t::channel_types::data[2]);
    LOG(rgb888_t::bytesize, sizeof(rgb888_t));

    LOG(rgba8888_t::channel_types::data[0], rgba8888_t::channel_types::data[1], rgba8888_t::channel_types::data[2], rgba8888_t::channel_types::data[3]);
    LOG(rgba8888_t::bytesize, sizeof(rgba8888_t));

    return 0;
}
