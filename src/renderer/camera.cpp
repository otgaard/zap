/* Created by Darren Otgaar on 2016/05/24. http://www.github.com/otgaard/zap */
#include "camera.hpp"
#include <tools/log.hpp>
#include <maths/io.hpp>

#include "engine/gl_api.hpp"

using namespace zap::maths;
using namespace zap::engine;
using namespace zap::renderer;
using namespace zap::engine::gl;

constexpr zap::maths::mat4f default_frame = {
        1.f, 0.f,  0.f, 0.f,
        0.f, 1.f,  0.f, 0.f,
        0.f, 0.f, -1.f, 0.f,
        0.f, 0.f,  0.f, 1.f
};

camera::camera(bool perspective) {
    block_.cam_world_to_view = mat4f{1.f, 1.f, -1.f, 1.f};
    block_.cam_view_to_world = default_frame;

    cam_state_.set(camera_state::CS_IDENTITY);
    if(perspective) {
        cam_state_.set(camera_state::CS_PERSPECTIVE);
        frustum(90.f, 1.f, 1.f, 100.f);
    } else {
        frustum(0.f, 1.f, 0.f, 1.f, 0.f, 1.f);
    }
    update_view();
}

camera::camera(const vec3f& up, const vec3f& dir, const vec3f& pos, bool perspective) { // : world_to_view_(1,1,-1,1) {
    block_.cam_world_to_view = mat4f{1.f, 1.f, -1.f, 1.f};
    block_.cam_view_to_world.frame(maths::cross(dir,up).normalise(), up, dir, pos);
    frustum(90.f, 1.f, 1.f, 100.f);
    update_view();
}

void camera::viewport(const viewport_t& vp) {
    block_.viewport = vp;
    glViewport((size_t)vp[0], (size_t)vp[1], (size_t)vp[2], (size_t)vp[3]);
}

void camera::update_view() {
    maths::mat3f R(block_.cam_view_to_world.col3(0), block_.cam_view_to_world.col3(1), block_.cam_view_to_world.col3(2));
    R.transpose();
    block_.cam_world_to_view.column(3, -(R * block_.cam_view_to_world.col3(3)));
    block_.cam_world_to_view.rotation(R);

    block_.cam_proj_view = block_.cam_projection * block_.cam_world_to_view;
    if(cam_state_.is_set(camera_state::CS_PRE_VIEW))  block_.cam_proj_view = block_.cam_proj_view * pre_view_;
    if(cam_state_.is_set(camera_state::CS_POST_VIEW)) block_.cam_proj_view = post_view_ * block_.cam_proj_view;
    block_.view_position = vec4f{world_pos(), 1.f};
    block_.view_dir = vec4f{-dir(), 0.f};
    seq_id_++;
    cam_state_.set(camera_state::CS_DIRTY);
}

void camera::update_frustum() {
    const auto& d_min = frustum_[FP_DMIN];
    const auto& d_max = frustum_[FP_DMAX];
    const auto& u_min = frustum_[FP_UMIN];
    const auto& u_max = frustum_[FP_UMAX];
    const auto& r_min = frustum_[FP_RMIN];
    const auto& r_max = frustum_[FP_RMAX];

    float inv_D_diff = 1.0f / (d_max - d_min);
    float inv_U_diff = 1.0f / (u_max - u_min);
    float inv_R_diff = 1.0f / (r_max - r_min);

    float sumD = (d_min + d_max) * inv_D_diff;
    float sumU = (u_min + u_max) * inv_U_diff;
    float sumR = (r_min + r_max) * inv_R_diff;

    if(cam_state_.is_set(camera_state::CS_PERSPECTIVE)) {
        float two_dmin_inv_R_diff = 2.0f * d_min * inv_R_diff;
        float two_dmin_inv_U_diff = 2.0f * d_min * inv_U_diff;
        float dmax_inv_D_diff = d_max * inv_D_diff;
        float dmin_dmax_inv_D_diff = d_min * dmax_inv_D_diff;
        float two_dmin_dmax_inv_D_diff = 2.0f * dmin_dmax_inv_D_diff;

        block_.cam_projection(0, 0) = two_dmin_inv_R_diff;
        block_.cam_projection(0, 1) = 0.0f;
        block_.cam_projection(0, 2) = -sumR;
        block_.cam_projection(0, 3) = 0.0f;
        block_.cam_projection(1, 0) = 0.0f;
        block_.cam_projection(1, 1) = two_dmin_inv_U_diff;
        block_.cam_projection(1, 2) = -sumU;
        block_.cam_projection(1, 3) = 0.0f;
        block_.cam_projection(2, 0) = 0.0f;
        block_.cam_projection(2, 1) = 0.0f;
        block_.cam_projection(2, 2) = sumD;
        block_.cam_projection(2, 3) = -two_dmin_dmax_inv_D_diff;
        block_.cam_projection(3, 0) = 0.0f;
        block_.cam_projection(3, 1) = 0.0f;
        block_.cam_projection(3, 2) = 1.0f;
        block_.cam_projection(3, 3) = 0.0f;
    } else {
        float two_inv_R_diff = 2.0f * inv_R_diff;
        float two_inv_U_diff = 2.0f * inv_U_diff;
        float two_inv_D_diff = 2.0f * inv_D_diff;

        block_.cam_projection(0, 0) = two_inv_R_diff;
        block_.cam_projection(0, 1) = 0.0f;
        block_.cam_projection(0, 2) = 0.0;
        block_.cam_projection(0, 3) = -sumR;
        block_.cam_projection(1, 0) = 0.0f;
        block_.cam_projection(1, 1) = two_inv_U_diff;
        block_.cam_projection(1, 2) = 0.0f;
        block_.cam_projection(1, 3) = -sumU;
        block_.cam_projection(2, 0) = 0.0f;
        block_.cam_projection(2, 1) = 0.0f;
        block_.cam_projection(2, 2) = two_inv_D_diff;
        block_.cam_projection(2, 3) = -sumD;
        block_.cam_projection(3, 0) = 0.0f;
        block_.cam_projection(3, 1) = 0.0f;
        block_.cam_projection(3, 2) = 0.0f;
        block_.cam_projection(3, 3) = 1.0f;
    }
    update_view();
}

bool camera::pick_ray(int x, int y, vec3f& origin, vec3f& d) const {
    if(x < block_.viewport[0] || x > block_.viewport[2] || y < block_.viewport[1] || y > block_.viewport[3]) return false;
    auto r = (x - block_.viewport[0])/block_.viewport[2]; auto u = (y - block_.viewport[1])/block_.viewport[3];
    float dr = maths::lerp(r, frustum_[FP_RMIN], frustum_[FP_RMAX]);
    float du = maths::lerp(u, frustum_[FP_UMIN], frustum_[FP_UMAX]);
    float dd = frustum_[FP_DMIN];

    origin = world_pos();
    d = dr*right() + du*up() + dd*dir();
    d.normalise();
    return true;
}

const std::string camera::uniform_block_def(const char* const term) {
    return
        std::string("layout (std140) uniform camera {") + term +
            "mat4 world_to_view;" + term +
            "mat4 view_to_world;" + term +
            "mat4 projection;" + term +
            "mat4 proj_view;" + term +
            "ivec4 viewport;" + term +
            "vec4 view_position;" + term +
            "vec4 view_dir;" + term +
        "};" + term;
}
