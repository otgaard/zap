/* Created by Darren Otgaar on 2016/06/10. http://www.github.com/otgaard/zap */
#ifndef ZAP_MODULE_HPP
#define ZAP_MODULE_HPP

#include <array>
#include <atomic>
#include <string>

/*
 * A switchable module for visualisation.
 */

class module {
public:
    using fft_analysis_t = std::array<float, 64>;

    module(const std::string& name) : name_(name), updated_(false) { }
    virtual ~module() { }

    inline void set_analysis(const fft_analysis_t& fft) { analysis_ = fft; }
    inline const fft_analysis_t& get_analysis() const { return analysis_; }

    virtual void update(double t, float dt) { }
    virtual void draw() { }

protected:
    std::string name_;
    fft_analysis_t analysis_;
    std::atomic<bool> updated_;
};

#endif //ZAP_MODULE_HPP
