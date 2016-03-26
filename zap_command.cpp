//
// Created by Darren Otgaar on 2016/03/20.
//

#include <tools/log.hpp>
#include <maths/vec3.hpp>
#include <chrono>
#include <vector>
#include <array>

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

int main(int argc, char* argv[]) {
    return 0;
}
