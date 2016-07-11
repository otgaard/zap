/* Created by Darren Otgaar on 2016/06/26. http://www.github.com/otgaard/zap */
#ifndef ZAP_PLOTTER2_HPP
#define ZAP_PLOTTER2_HPP

#include <array>
#include <memory>
#include <maths/vec2.hpp>
#include <maths/mat4.hpp>

namespace zap { namespace graphics2 {
    using mat4f = maths::mat4f;
    using vec2f = maths::vec2f;

class plotter2 {
public:
    plotter2();
    ~plotter2();

    bool initialise();
    void draw();

    void set_projection_matrix(const mat4f& proj_mat);
    void set_mv_matrix(const mat4f& mv_mat);

    template <typename CurveFnc>
    void sample_curve(CurveFnc curve, float start, float end) {
        const float du = (end - start)/(1000 - 1);
        std::array<vec2f, 1000> data;
        for(size_t i = 0; i != 1000; ++i) {
            data[i] = curve(start + i*du);
        }
        set_data(data);
    }

    void push_value(float value);
    void set_data(const std::array<float,1000>& data);
    void set_data(const std::array<vec2f,1000>& data);

    // Change this interface to a transform and render with an orthographic projection
    void transform(const mat4f& transform);

private:
    struct state_t;
    std::unique_ptr<state_t> state_;
};

}}

#endif //ZAP_PLOTTER2_HPP
