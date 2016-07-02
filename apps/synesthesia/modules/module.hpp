/* Created by Darren Otgaar on 2016/06/10. http://www.github.com/otgaard/zap */
#ifndef ZAP_MODULE_HPP
#define ZAP_MODULE_HPP

#include <array>
#include <atomic>
#include <string>
#include <apps/synesthesia/spectral_analyser.hpp>

/*
 * A switchable module for visualisation.
 */

class application;

class module {
public:
    using fft_window = spectral_analyser::fft_window;

    module(application* app_ptr, const std::string& name) : app_ptr_(app_ptr), name_(name), updated_(false) {
        for(auto& v : window_) v = 0.f;
    }
    virtual ~module() { }

    inline void set_window(const fft_window& fft) { std::copy(fft.begin(), fft.end(), window_.begin()); }
    inline const fft_window& get_window() const { return window_; }
    inline void set_analysis(const analysis& a) { analysis_ = a; }
    inline const analysis& get_analysis() const { return analysis_; }

    virtual void update(double t, float dt) { }
    virtual void draw() { }

protected:
    application* app_ptr_;
    std::string name_;
    fft_window window_;
    analysis analysis_;
    std::atomic<bool> updated_;
};

#endif //ZAP_MODULE_HPP
