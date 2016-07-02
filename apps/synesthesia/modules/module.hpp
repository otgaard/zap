/* Created by Darren Otgaar on 2016/06/10. http://www.github.com/otgaard/zap */
#ifndef ZAP_MODULE_HPP
#define ZAP_MODULE_HPP

#include <array>
#include <atomic>
#include <string>

/*
 * A switchable module for visualisation.
 */

class application;

class module {
public:
    using fft_analysis_t = std::array<float, 64>;

    module(application* app_ptr, const std::string& name) : app_ptr_(app_ptr), name_(name), updated_(false) {
        for(auto& v : analysis_) v = 0.f;
    }
    virtual ~module() { }

    inline void set_analysis(const fft_analysis_t& fft) { std::copy(fft.begin(), fft.end(), analysis_.begin()); }
    inline const fft_analysis_t& get_analysis() const { return analysis_; }

    virtual void update(double t, float dt) { }
    virtual void draw() { }

protected:
    application* app_ptr_;
    std::string name_;
    fft_analysis_t analysis_;
    std::atomic<bool> updated_;
};

#endif //ZAP_MODULE_HPP
