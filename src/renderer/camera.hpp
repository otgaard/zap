/* Created by Darren Otgaar on 2016/05/24. http://www.github.com/otgaard/zap */
#ifndef ZAP_CAMERA_HPP
#define ZAP_CAMERA_HPP

#include <renderer/rndr.hpp>
#include <maths/algebra.hpp>
#include <core/enumfield.hpp>
#include <engine/uniform_buffer.hpp>

namespace zap { namespace renderer {
    using camera_block = engine::uniform_block<
            core::cam_world_to_view<maths::mat4f>,
            core::cam_view_to_world<maths::mat4f>,
            core::cam_projection<maths::mat4f>,
            core::cam_proj_view<maths::mat4f>,
            core::viewport<maths::vec4i>,
            core::view_position<maths::vec4f>,
            core::view_dir<maths::vec4f>
    >;

    using camera_ubuffer = engine::uniform_buffer<camera_block>;

    class ZAPRENDERER_EXPORT camera {
    protected:
        enum class camera_state {
            CS_IDENTITY = 1 << 0,        // The camera is the identity matrix
            CS_PERSPECTIVE = 1 << 1,     // The camera is using perspective projection
            CS_PRE_VIEW = 1 << 2,        // The camera requires a pre-view matrix multiplication
            CS_POST_VIEW = 1 << 3,       // The camera requires a post-view matrix multiplication
            CS_DIRTY = 1 << 4            // The camera state has changed since the last clear
        };

    public:
        using vec2f = maths::vec2f;
        using vec2i = maths::vec2i;
        using vec3f = maths::vec3f;
        using vec4f = maths::vec4f;
        using vec4i = maths::vec4i;
        using mat4f = maths::mat4f;
        using frustum_t = maths::vector<float, 6>;
        using viewport_t = maths::vec4i;

        camera(bool perspective=true);
        camera(const vec3f& up, const vec3f& dir, const vec3f& pos, bool perspective=true);

        bool is_perspective() const { return cam_state_.is_set(camera_state::CS_PERSPECTIVE); }
        void set_perspective(bool p) { p ? cam_state_.set(camera_state::CS_PERSPECTIVE) : cam_state_.clear(camera_state::CS_PERSPECTIVE); }
        vec3f right() const { return block_.cam_view_to_world.col3(0); }
        vec3f up() const { return block_.cam_view_to_world.col3(1); }
        vec3f dir() const { return block_.cam_view_to_world.col3(2); }
        vec3f world_pos() const { return block_.cam_view_to_world.col3(3); }
        vec3f local_pos() const { return block_.cam_world_to_view.col3(3); }
        void right(const vec3f& r) { block_.cam_view_to_world.column(0,r); update_view(); }
        void up(const vec3f& u) { block_.cam_view_to_world.column(1,u); update_view(); }
        void dir(const vec3f& d) { block_.cam_view_to_world.column(2,d); update_view(); }
        void world_pos(const vec3f& P) { block_.cam_view_to_world.column(3,P); update_view(); }
        void world_pos(float x, float y, float z) { block_.cam_view_to_world.column(3,vec3f{x, y, z}); update_view(); }

        bool is_dirty() const { return cam_state_.is_set(camera_state::CS_DIRTY); }
        void clear_dirty() const { cam_state_.clear(camera_state::CS_DIRTY); }

        void frame(const vec3f& u, const vec3f& d, const vec3f& P) {
            assert(maths::is_zero(maths::dot(u, d), 2*std::numeric_limits<float>::epsilon()) && "Frame requires orthogonal vectors");
            block_.cam_view_to_world.column(0, maths::cross(d, u));
            block_.cam_view_to_world.column(1,u);
            block_.cam_view_to_world.column(2,d);
            block_.cam_view_to_world.column(3,P);
            update_view();
        }

        void look_at(const vec3f& P, const vec3f& world_up=vec3f{0.f, 1.f, 0.f}) {
            orthogonolise(maths::normalise(P - world_pos()), world_up);
        }

        void orthogonolise(const vec3f& d, const vec3f& world_up=vec3f{0.f, 1.f, 0.f}) {
            assert(maths::eq(d.length_sqr(), 1.f, 8*std::numeric_limits<float>::epsilon()) && "Direction vector must be unit length");
            auto up = maths::normalise(world_up - maths::dot(world_up, d)*d);
            block_.cam_view_to_world.column(2, d);
            block_.cam_view_to_world.column(1, up);
            block_.cam_view_to_world.column(0, maths::cross(d, up));
            update_view();
        }

        const mat4f& projection() const { return block_.cam_projection; }
        const mat4f& proj_view() const { return block_.cam_proj_view; }
        const mat4f& world_to_view() const { return block_.cam_world_to_view; }
        const mat4f& view_to_world() const { return block_.cam_view_to_world; }

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

        void viewport(int left, int bottom, int right, int top) {
            viewport(viewport_t({{left, bottom, right, top}}));
        }
        void viewport(const viewport_t& vp);
        viewport_t viewport() const { return block_.viewport; }

        int width() const { return block_.viewport[2] - block_.viewport[0]; }
        int height() const { return block_.viewport[3] - block_.viewport[1]; }
        vec2i dims() const { return vec2i(width(), height()); }

        bool pick_ray(int x, int y, vec3f& origin, vec3f& dir) const;

        const camera_block& get_uniform_block(bool clear=true) const { if(clear) clear_dirty(); return block_; }
        static const std::string uniform_block_def(const char* const term="");

        size_t sequence_id() const { return seq_id_; }

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

        camera_block block_ = { };

        mat4f pre_view_;
        mat4f post_view_;
        frustum_t frustum_;
        mutable core::enumfield<int, camera_state> cam_state_;
        size_t seq_id_ = 0; // Updated per view_update
    };
}}

#endif //ZAP_CAMERA_HPP
