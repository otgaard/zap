//
// Created by Darren Otgaar on 2016/04/10.
//

#ifndef ZAP_FUNCTIONS_HPP
#define ZAP_FUNCTIONS_HPP

#include "maths/maths.hpp"
#include <chrono>

namespace zap { namespace maths {

struct timer {
    enum { STOPPED, RUNNING, PAUSED } state;
    std::chrono::high_resolution_clock::time_point start_time;
    timer(bool start=false) : state(start ? RUNNING : STOPPED), start_time(std::chrono::high_resolution_clock::now()) {}

    bool is_stopped() const { return state == STOPPED; }
    bool is_running() const { return state == RUNNING; }
    bool is_paused() const { return state == PAUSED; }

    void reset(bool start=true) {
        start_time = std::chrono::high_resolution_clock::now();
        state = start ? RUNNING : STOPPED;
    }
    void start() {
        if(is_stopped()) start_time = std::chrono::high_resolution_clock::now();
        state = RUNNING;
    }

    void stop() { state = STOPPED; }
    void pause() { state = PAUSED; }

    template <typename T> T get_time() const {
        auto end_time = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<T>(end_time - start_time).count();
    }
};

template <typename T>
class oscillator {
public:
    static_assert(std::is_floating_point<T>::value, ZERR_TYPE_FLOATING);
    oscillator(T freq) : inv_freq(T(1)/freq), time(false) { }

    void start() { time.start(); }
    T get_value() const { return std::fmod(time.get_time<T>(),inv_freq)/inv_freq; }

private:
    T inv_freq;
    timer time;
};

}}

#endif //ZAP_FUNCTIONS_HPP
