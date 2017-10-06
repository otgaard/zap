//
// Created by otgaard on 2017/05/24.
//

#include <map>
#include <maths/io.hpp>
#include <tools/log.hpp>
#include <maths/geometry/AABB.hpp>
#include <generators/textures/planar.hpp>
#include <engine/texture.hpp>
#include <engine/program.hpp>
#include <renderer/camera.hpp>
#include <renderer/colour.hpp>
#include <maths/rand_lcg.hpp>
#include <engine/sampler.hpp>
#include <maths/geometry/sphere.hpp>
#include <maths/geometry/ray.hpp>
#include <engine/state_stack.hpp>
#include "host/GLFW/application.hpp"
#include "graphics3/g3_types.hpp"
#include "generators/geometry/geometry3.hpp"

// Frustum-culling spheres

using namespace zap;
using namespace zap::maths;
using namespace zap::graphics;
using namespace zap::renderer;
using namespace zap::generators;
using namespace zap::maths::geometry;

class frustum : public application {
public:
    frustum() : application{"frustum", 1280, 768} { }

    bool initialise() override final;
    void update(double t, float dt) override final;
    void draw() override final;
    void shutdown() override final;

    void on_resize(int width, int height) override final;

    void on_mousedown(int button) override;

    void on_mouseup(int button) override;

    void on_mousemove(double x, double y) override final;
    void on_mousewheel(double xinc, double yinc) override final;

protected:
    void make_textured_sphere();

    bool initialise_buffers();
    bool initialise_programs();

    vbuf_p3n3t2_t tex_vb_;
    ibuf_tri4_t tex_ib_;
    mesh_p3n3t2_trii_t tex_mesh_;

    texture check_tex_;
    program prog_tex_;

    camera side_cam_;
    camera front_cam_;

    std::vector<vec3f> spheres_;
    std::vector<int> selected_;

    sampler sampler_;

    state_stack rndr_state_;
};

const char* const tex_vtx_shdr = GLSL(
        uniform mat4 pvm;
        uniform vec3 spheres[5*5*5];
        uniform int selected[5*5*5];

        in vec3 position;
        in vec3 normal;
        in vec2 texcoord1;

        out vec3 nor;
        out vec2 tex;
        flat out int is_selected;

        void main() {
            nor = normal;
            tex = texcoord1;
            is_selected = selected[gl_InstanceID];
            gl_Position = pvm * vec4((position + spheres[gl_InstanceID]), 1);
        }
);

const char* const tex_frg_shdr = GLSL(
        const vec3 ld = vec3(0, 0.707, 0.707);

        uniform sampler2D diffuse_tex;

        flat in int is_selected;
        in vec3 nor;
        in vec2 tex;

        out vec4 frag_colour;

        void main() {
            float s = max(dot(nor, ld), 0) + .2;
            frag_colour = is_selected == 0 ? vec4(s * texture(diffuse_tex, tex).gbr, 1) : vec4(s * texture(diffuse_tex, tex).rgb, 1);
        }
);

bool frustum::initialise_programs() {
    prog_tex_.add_shader(shader_type::ST_VERTEX, tex_vtx_shdr);
    prog_tex_.add_shader(shader_type::ST_FRAGMENT, tex_frg_shdr);
    if(!prog_tex_.link()) {
        LOG_ERR("Error linking tex shader");
        return false;
    }

    prog_tex_.bind();
    prog_tex_.bind_texture_unit("diffuse_tex", 0);
    prog_tex_.release();
    return true;
}

bool frustum::initialise_buffers() {
    if(!tex_vb_.allocate() || !tex_ib_.allocate() || !tex_mesh_.allocate()) {
        LOG_ERR("Could not allocate textured mesh resources");
        return false;
    }

    tex_mesh_.set_stream(&tex_vb_); tex_mesh_.set_index(&tex_ib_);
    make_textured_sphere();
    return true;
}

bool frustum::initialise() {
    front_cam_.viewport(0, 0, 1280, 768);
    front_cam_.frustum(45.f, 1280.f/768.f, .5f, 100.f);
    front_cam_.world_pos(vec3f(0,0,5));
    front_cam_.orthogonolise(vec3f(0,0,-1));

    if(!rndr_state_.initialise()) {
        LOG_ERR("Failed to initialise render state");
        return false;
    }

    rndr_state_.clear_colour(.15f, .15f, .15f, .15f);

    if(!initialise_programs()) {
        LOG_ERR("Failed to initialise program resources for example");
        return false;
    }

    if(!initialise_buffers()) {
        LOG_ERR("Failed to initialise buffer resources for example");
        return false;
    }

    auto pixels = planar<rgb888_t>::make_checker(16, 8, colour::blue8, colour::green8);
    check_tex_.allocate();
    check_tex_.initialise(16, 8, pixels, false);

    rand_lcg rand;

    // Create an array of spheres for picking
    spheres_.resize(5*5*5);
    selected_.resize(5*5*5);
    for(auto r = 0; r != 5; ++r) {
        for(auto c = 0; c  != 5; ++c) {
            for(auto d = 0; d != 5; ++d){
                spheres_[25*r + 5*c + d] = vec3f{c - 2.f, r - 2.f, d - 2.f};
                selected_[25*r + 5*c + d] = false;
            }
        }
    }

    prog_tex_.bind();
    prog_tex_.bind_uniform("spheres", spheres_);
    prog_tex_.bind_uniform("selected", selected_);
    prog_tex_.release();

    gl_error_check();

    sampler_.allocate();
    sampler_.set_wrap_s(tex_wrap::TW_CLAMP_TO_BORDER);
    sampler_.set_wrap_t(tex_wrap::TW_CLAMP_TO_BORDER);
    sampler_.set_min_filter(tex_filter::TF_LINEAR);
    sampler_.set_mag_filter(tex_filter::TF_LINEAR);
    sampler_.set_border_colour(1.f, 1.f, 1.f, 1.f);

    gl_error_check();

    return true;
}

void frustum::on_resize(int width, int height) {
    application::on_resize(width, height);
    front_cam_.viewport(0, 0, width, height);
    front_cam_.frustum(45.f, width/float(height), .5f, 100.f);
    front_cam_.world_pos(vec3f(0,0,10));
    front_cam_.orthogonolise(vec3f(0,0,-1));
}

void frustum::on_mousemove(double x, double y) {
    application::on_mousemove(x, y);
}

void frustum::on_mousewheel(double xinc, double yinc) {
    application::on_mousewheel(xinc, yinc);
}

float inc = 0;

void frustum::update(double t, float dt) {
    auto rot = make_rotation(vec3f(0,1,0), inc);

    prog_tex_.bind();
    prog_tex_.bind_uniform("pvm", front_cam_.proj_view() * rot);
    prog_tex_.release();

    inc += 0.001f;
}

void frustum::draw() {
    prog_tex_.bind();
    check_tex_.bind(0);
    sampler_.bind(0);
    tex_mesh_.bind();
    tex_mesh_.draw_inst(5*5*5);
    tex_mesh_.release();
    sampler_.release(0);
    check_tex_.release();
    prog_tex_.release();
}

void frustum::shutdown() {
}

int main(int argc, char* argv[]) {
    frustum app;
    return app.run();
}
void frustum::make_textured_sphere() {
    std::vector<vtx_p3n3t2_t> vbuf;
    std::vector<uint32_t> ibuf;
    std::tie(vbuf, ibuf) = geometry3<vtx_p3n3t2_t, primitive_type::PT_TRIANGLES>::make_UVsphere<float,uint32_t>(15, 30, .2f, false);
    tex_mesh_.bind();
    tex_vb_.bind();
    tex_vb_.initialise(vbuf);
    tex_ib_.bind();
    tex_ib_.initialise(ibuf);
    tex_mesh_.release();
}

void frustum::on_mousedown(int button) {
    application::on_mousedown(button);
    LOG("down:", mouse_);

    auto rot = make_rotation(vec3f(0,1,0), inc);

    vec3f O, d;
    if(!front_cam_.pick_ray(mouse_.x, mouse_.y, O, d)) return;

    float parms[2];
    for(size_t i = 0; i != spheres_.size(); ++i) {
        auto sp = zap::maths::geometry::sphere<float>(rot.transform(spheres_[i]), .2f);
        if(intersection(sp, zap::maths::geometry::ray3f(O, d), parms) > 0) selected_[i] = true;
        else                                                               selected_[i] = false;
    }

    prog_tex_.bind();
    prog_tex_.bind_uniform("selected", selected_);
    prog_tex_.release();
}

void frustum::on_mouseup(int button) {
    application::on_mouseup(button);
    LOG("up:", mouse_);
}


