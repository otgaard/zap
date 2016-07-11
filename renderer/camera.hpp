/* Created by Darren Otgaar on 2016/05/24. http://www.github.com/otgaard/zap */
#ifndef ZAP_CAMERA_HPP
#define ZAP_CAMERA_HPP

#include <maths/maths.hpp>
#include <maths/vector.hpp>
#include <maths/vec2.hpp>
#include <maths/vec3.hpp>
#include <maths/vec4.hpp>
#include <maths/mat2.hpp>
#include <maths/mat3.hpp>
#include <maths/mat4.hpp>
#include <core/enumfield.hpp>

namespace zap { namespace renderer {
    class camera {
    public:
        using vec3f = maths::vec3f;
        using vec4f = maths::vec4f;
        using mat4f = maths::mat4f;
        using frustum_t = maths::vector<float, 6>;
        using viewport_t = maths::vector<float, 4>;

        camera(bool perspective=true);
        camera(const vec3f& up, const vec3f& dir, const vec3f& pos, bool perspective=true);

        vec3f right() const { return view_to_world_.col3(0); }
        vec3f up() const { return view_to_world_.col3(1); }
        vec3f dir() const { return view_to_world_.col3(2); }
        vec3f world_pos() const { return view_to_world_.col3(3); }
        vec3f local_pos() const { return world_to_view_.col3(3); }
        void right(const vec3f& r) { view_to_world_.column(0,r); on_view_changed(); }
        void up(const vec3f& u) { view_to_world_.column(1,u); on_view_changed(); }
        void dir(const vec3f& d) { view_to_world_.column(2,d); on_view_changed(); }
        void world_pos(const vec3f& P) { view_to_world_.column(3,P); on_view_changed(); }
        void frame(const vec3f& u, const vec3f& d, const vec3f& P) {
            view_to_world_.column(1,u); view_to_world_.column(2,d), view_to_world_.column(3,P);
            on_view_changed();
        }



    protected:
        enum class camera_state {
            CS_IS_IDENTITY = 1 << 0,        // The camera is the identity matrix
            CS_IS_PROJECTION = 1 << 1,      // The camera is using perspective projection
            CS_IS_PRE_VIEW = 1 << 2,        // The camera requires a pre-view matrix multiplication
            CS_IS_POST_VIEW = 1 << 3        // The camera requires a post-view matrix multiplication
        };

        void on_view_changed();
        void on_frustum_changed();
        void on_viewport_changed();

        mat4f world_to_view_;
        mat4f view_to_world_;
        mat4f projection_;
        mat4f projection_view_;
        mat4f pre_view_;
        mat4f post_view_;
        frustum_t frustum_;
        viewport_t viewport_;
        core::enumfield<int, camera_state> cam_state_;
    };
}}

#endif //ZAP_CAMERA_HPP
