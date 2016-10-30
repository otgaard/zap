/* Created by Darren Otgaar on 2016/05/24. http://www.github.com/otgaard/zap */
#ifndef ZAP_CAMERA_HPP
#define ZAP_CAMERA_HPP

#include <maths/algebra.hpp>
#include <core/enumfield.hpp>

namespace zap { namespace renderer {
    class camera {
    protected:
        enum class camera_state {
            CS_IDENTITY = 1 << 0,        // The camera is the identity matrix
            CS_PERSPECTIVE = 1 << 1,     // The camera is using perspective projection
            CS_PRE_VIEW = 1 << 2,        // The camera requires a pre-view matrix multiplication
            CS_POST_VIEW = 1 << 3        // The camera requires a post-view matrix multiplication
        };

    public:
        using vec2f = maths::vec2f;
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
        void right(const vec3f& r) { view_to_world_.column(0,r); update_view(); }
        void up(const vec3f& u) { view_to_world_.column(1,u); update_view(); }
        void dir(const vec3f& d) { view_to_world_.column(2,d); update_view(); }
        void world_pos(const vec3f& P) { view_to_world_.column(3,P); update_view(); }

        void frame(const vec3f& u, const vec3f& d, const vec3f& P) {
            assert(maths::is_zero(maths::dot(u, d), 2*std::numeric_limits<float>::epsilon()) && "Frame requires orthogonal vectors");
            view_to_world_.column(0, maths::cross(d, u));
            view_to_world_.column(1,u);
            view_to_world_.column(2,d);
            view_to_world_.column(3,P);
            update_view();
        }

        void orthogonolise(const vec3f& d, const vec3f& world_up=vec3f(0.f, 1.f, 0.f)) {
            assert(maths::eq(d.length_sqr(), 1.f, 2*std::numeric_limits<float>::epsilon()) && "Direction vector must be unit length");
            auto up = maths::normalise(world_up - maths::dot(world_up, d)*d);
            view_to_world_.column(2, d);
            view_to_world_.column(1, up);
            view_to_world_.column(0, maths::cross(d, up));
            update_view();
        }

        const mat4f& projection() const { return projection_; }
        const mat4f& proj_view() const { return projection_view_; }
        const mat4f& world_to_view() const { return world_to_view_; }
        const mat4f& view_to_world() const { return view_to_world_; }

        void frustum(float r_min, float r_max, float u_min, float u_max, float d_min, float d_max) {
            cam_state_.clear(camera_state::CS_PERSPECTIVE);
            frustum_[FP_DMIN] = d_min; frustum_[FP_DMAX] = d_max;
            frustum_[FP_UMIN] = u_min; frustum_[FP_UMAX] = u_max;
            frustum_[FP_RMIN] = r_min; frustum_[FP_RMAX] = r_max;
            update_frustum();
        }

        void frustum(float fov, float ar, float d_min, float d_max) {
            float ha_rad = .5f * maths::deg_to_rad(fov);
            frustum_[FP_UMAX] = d_min * std::tan(ha_rad);
            frustum_[FP_RMAX] = ar * frustum_[FP_UMAX];
            frustum_[FP_UMIN] = -frustum_[FP_UMAX];
            frustum_[FP_RMIN] = -frustum_[FP_RMAX];
            frustum_[FP_DMAX] = d_max;
            frustum_[FP_DMIN] = d_min;
            update_frustum();
        }

        void viewport(float left, float bottom, float right, float top) {
            viewport(viewport_t({{left, bottom, right, top}}));
        }
        void viewport(const viewport_t& vp);
        viewport_t viewport() const { return viewport_; }

        float width() const { return viewport_[2] - viewport_[0]; }
        float height() const { return viewport_[3] - viewport_[1]; }
        vec2f dims() const { return vec2f(width(), height()); }

        bool pick_ray(int x, int y, vec3f& origin, vec3f& dir) const;

    protected:
        enum frustum_plane {
            FP_DMIN = 0,
            FP_DMAX = 1,
            FP_UMIN = 2,
            FP_UMAX = 3,
            FP_RMIN = 4,
            FP_RMAX = 5
        };

        void update_view();
        void update_frustum();

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
