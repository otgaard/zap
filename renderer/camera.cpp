/* Created by Darren Otgaar on 2016/05/24. http://www.github.com/otgaard/zap */
#include "camera.hpp"
#include <maths/mat3.hpp>

#define LOGGING_ENABLED
#include <tools/log.hpp>
#include <maths/io.hpp>

using namespace zap::maths;
using namespace zap::renderer;

constexpr zap::maths::mat4f default_frame = {
        1.f, 0.f,  0.f, 0.f,
        0.f, 1.f,  0.f, 0.f,
        0.f, 0.f, -1.f, 0.f,
        0.f, 0.f,  0.f, 1.f
};

camera::camera(bool perspective) : world_to_view_(1,1,-1,1), view_to_world_(default_frame) {
    cam_state_.set(camera_state::CS_IDENTITY);
    if(perspective) {
        cam_state_.set(camera_state::CS_PERSPECTIVE);
        frustum(90.f, 1.f, 1.f, 100.f);
    } else {
        frustum(0.f, 1.f, 0.f, 1.f, 0.f, 1.f);
    }
    update_view();
}

camera::camera(const vec3f& up, const vec3f& dir, const vec3f& pos, bool perspective) : world_to_view_(1,1,-1,1) {
    view_to_world_.frame(maths::cross(dir,up).normalise(), up, dir, pos);
    frustum(90.f, 1.f, 1.f, 100.f);
    update_view();
}

void camera::update_view() {
    maths::mat3f R(view_to_world_.col3(0), view_to_world_.col3(1), view_to_world_.col3(2));
    R.transpose();
    world_to_view_.column(3, -(R * view_to_world_.col3(3)));
    world_to_view_.rotation(R);

    projection_view_ = projection_ * world_to_view_;
    if(cam_state_.is_set(camera_state::CS_PRE_VIEW))  projection_view_ = projection_view_ * pre_view_;
    if(cam_state_.is_set(camera_state::CS_POST_VIEW)) projection_view_ = post_view_ * projection_view_;
}

void camera::update_frustum() {
    const float& d_min = frustum_[FP_DMIN];
    const float& d_max = frustum_[FP_DMAX];
    const float& u_min = frustum_[FP_UMIN];
    const float& u_max = frustum_[FP_UMAX];
    const float& r_min = frustum_[FP_RMIN];
    const float& r_max = frustum_[FP_RMAX];

    float inv_D_diff = 1.0f / (d_max - d_min);
    float inv_U_diff = 1.0f / (u_max - u_min);
    float inv_R_diff = 1.0f / (r_max - r_min);

    float sumR = (r_min + r_max) * inv_R_diff;
    float sumU = (u_min + u_max) * inv_U_diff;
    float sumD = (d_min + d_max) * inv_D_diff;

    if(cam_state_.is_set(camera_state::CS_PERSPECTIVE)) {
        float two_dmin_inv_R_diff = 2.0f * d_min * inv_R_diff;
        float two_dmin_inv_U_diff = 2.0f * d_min * inv_U_diff;
        float dmax_inv_D_diff = d_max * inv_D_diff;
        float dmin_dmax_inv_D_diff = d_min * dmax_inv_D_diff;
        float two_dmin_dmax_inv_D_diff = 2.0f * dmin_dmax_inv_D_diff;

        projection_(0, 0) = two_dmin_inv_R_diff;
        projection_(0, 1) = 0.0f;
        projection_(0, 2) = -sumR;
        projection_(0, 3) = 0.0f;
        projection_(1, 0) = 0.0f;
        projection_(1, 1) = two_dmin_inv_U_diff;
        projection_(1, 2) = -sumU;
        projection_(1, 3) = 0.0f;
        projection_(2, 0) = 0.0f;
        projection_(2, 1) = 0.0f;
        projection_(2, 2) = sumD;
        projection_(2, 3) = -two_dmin_dmax_inv_D_diff;
        projection_(3, 0) = 0.0f;
        projection_(3, 1) = 0.0f;
        projection_(3, 2) = 1.0f;
        projection_(3, 3) = 0.0f;
    } else {
        float two_inv_R_diff = 2.0f * inv_R_diff;
        float two_inv_U_diff = 2.0f * inv_U_diff;
        float two_inv_D_diff = 2.0f * inv_D_diff;

        projection_(0, 0) = two_inv_R_diff;
        projection_(0, 1) = 0.0f;
        projection_(0, 2) = 0.0;
        projection_(0, 3) = -sumR;
        projection_(1, 0) = 0.0f;
        projection_(1, 1) = two_inv_U_diff;
        projection_(1, 2) = 0.0f;
        projection_(1, 3) = -sumU;
        projection_(2, 0) = 0.0f;
        projection_(2, 1) = 0.0f;
        projection_(2, 2) = two_inv_D_diff;
        projection_(2, 3) = -sumD;
        projection_(3, 0) = 0.0f;
        projection_(3, 1) = 0.0f;
        projection_(3, 2) = 0.0f;
        projection_(3, 3) = 1.0f;
    }
    update_view();
}

bool camera::pick_ray(int x, int y, vec3f& origin, vec3f& d) const {
    if(x < viewport_[0] || x > viewport_[2] || y < viewport_[1] || y > viewport_[3]) return false;
    auto r = (x - viewport_[0])/viewport_[2]; auto u = (y - viewport_[1])/viewport_[3];
    float dr = maths::lerp(r, frustum_[FP_RMIN], frustum_[FP_RMAX]);
    float du = maths::lerp(u, frustum_[FP_UMIN], frustum_[FP_UMAX]);
    float dd = frustum_[FP_DMIN];

    origin = world_pos();
    d = dd*dir() + dr*right() - du*up();
    d.normalise();
    return true;
}
