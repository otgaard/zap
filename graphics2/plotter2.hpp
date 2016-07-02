/* Created by Darren Otgaar on 2016/06/26. http://www.github.com/otgaard/zap */
#ifndef ZAP_PLOTTER2_HPP
#define ZAP_PLOTTER2_HPP

#include <array>
#include <memory>
#include <maths/mat4.hpp>

namespace zap { namespace graphics2 {
    using mat4f = maths::mat4f;

class plotter2 {
public:
    plotter2();
    ~plotter2();

    bool initialise();
    void draw();

    void push_value(float value);
    void set_data(const std::array<float,1000>& data);

    // Change this interface to a transform and render with an orthographic projection
    void transform(const mat4f& transform);

private:
    struct state_t;
    std::unique_ptr<state_t> state_;
};

}}

#endif //ZAP_PLOTTER2_HPP
