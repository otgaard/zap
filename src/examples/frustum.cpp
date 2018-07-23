//
// Created by otgaard on 2017/05/24.
//

#include <map>
#include <maths/io.hpp>
#include <tools/log.hpp>
#include <maths/geometry/AABB.hpp>
#include <graphics/generators/textures/planar.hpp>
#include <engine/texture.hpp>
#include <engine/program.hpp>
#include <renderer/camera.hpp>
#include <graphics/colour.hpp>
#include <maths/rand_lcg.hpp>
#include <engine/sampler.hpp>
#include <maths/geometry/sphere.hpp>
#include <maths/geometry/ray.hpp>
#include <engine/state_stack.hpp>
#include "host/GLFW/application.hpp"
#include "graphics/graphics3/g3_types.hpp"
#include "graphics/generators/geometry/geometry3.hpp"
#include "renderer/render_batch.hpp"
#include "maths/geometry/plane.hpp"

// Frustum-culling spheres

using namespace zap;
using namespace zap::maths;
using namespace zap::graphics;
using namespace zap::renderer;
using namespace zap::generators;
using namespace zap::maths::geometry;

using plane_batch = render_batch<mesh_p3n3t2_u32_t::vertex_stream_t, ibuf_u32_t>;

class frustum : public application {
public:
    frustum() : application{"frustum", 1280, 768} { }

    bool initialise() override final;
    void update(double t, float dt) override final;
    void draw() override final;
    void shutdown() override final;

    void on_resize(int width, int height) final;

    void on_mousedown(int button) final;

    void on_mouseup(int button) final;

    void on_mousemove(double x, double y) override final;
    void on_mousewheel(double xinc, double yinc) override final;

protected:
    void make_textured_sphere();
    void build_planes();
    void clip_to_planes();

    bool initialise_buffers();
    bool initialise_programs();

    vbuf_p3n3t2_t tex_vb_;
    ibuf_u32_t tex_ib_;
    mesh_p3n3t2_u32_t tex_mesh_;

    texture check_tex_;
    program prog_tex_;

    camera lside_cam_;
    camera top_cam_;
    camera world_cam_;
    camera rside_cam_;

    std::vector<vec3f> spheres_;
    std::vector<int> selected_;

    sampler sampler_;

    state_stack rndr_state_;

    std::vector<plane<float>> planes_;
    std::vector<plane_batch::token> tokens_;
    plane_batch plane_batch_;
    program plane_shdr_;
    render_state plane_state_;
};

const char* const basic_vtx_shdr = GLSL(
    uniform mat4 mvp;

    in vec3 position;
    in vec3 normal;

    out vec3 nor;

    void main() {
        nor = normal;
        gl_Position = mvp * vec4(position, 1.);
    }
);

const char* const basic_frg_shdr = GLSL(
    in vec3 nor;
    out vec4 frag_colour;

    void main() {
        frag_colour = vec4(1., 1., 1., 1.);
    }
);

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
    const vec3 ld = vec3(0., .707, .707);

    uniform sampler2D diffuse_tex;

    flat in int is_selected;
    in vec3 nor;
    in vec2 tex;

    out vec4 frag_colour;

    void main() {
        float s = max(dot(nor, ld), 0) + .2;
        frag_colour = mix(vec4(s * texture(diffuse_tex, tex).gbr, 1), vec4(s * texture(diffuse_tex, tex).rgb, 1), is_selected);
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
    if(!plane_batch_.initialise(4*32, 6*32)) {
        LOG_ERR("Failed to initialise plane batch");
        return false;
    }

    if(!plane_shdr_.link(basic_vtx_shdr, basic_frg_shdr)) {
        LOG_ERR("Failed to initialise plane shader");
        return false;
    }

    plane_state_.initialise(RS_DEPTH | RS_RASTERISATION);
    plane_state_.depth()->enabled = true;
    plane_state_.rasterisation()->poly_mode = rasterisation_state::polygon_mode::PM_LINE;
    plane_state_.rasterisation()->enable_culling = true;

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

float inc = 0;

void frustum::on_resize(int width, int height) {
    const auto target = vec3f(2.f, 0.f, 0.f);
    const auto rot = make_rotation(vec3f{0.f, 0.f, -1.f}, inc);
    const auto lP = rot.transform(target);

    application::on_resize(width, height);
    world_cam_.viewport(0, 0, width/2, height/2);
    world_cam_.frustum(45.f, width/float(height), .5f, 7.5f);
    world_cam_.world_pos(vec3f{0.f, 0.f, 5.f});
    world_cam_.look_at(lP);

    lside_cam_.viewport(width/2, 0, width/2, height/2);
    lside_cam_.frustum(45.f, width/float(height), .5f, 100.f);
    lside_cam_.world_pos(vec3f{-20.f, 0.f, 0.f});
    lside_cam_.look_at(vec3f{0.f, 0.f, 0.f});

    top_cam_.viewport(0, height/2, width/2, height/2);
    top_cam_.frustum(90.f, width/float(height), .5f, 50.f);
    top_cam_.world_pos(vec3f{0.f, 5.f, 0.f});
    top_cam_.look_at(vec3f{0.f, 0.f, 0.f}, vec3f{0.f, 0.f, -1.f});

    rside_cam_.viewport(width/2, height/2, width/2, height/2);
    rside_cam_.frustum(45.f, width/float(height), .5f, 100.f);
    rside_cam_.world_pos(vec3f{20.f, 0.f, 0.f});
    rside_cam_.look_at(vec3f{0.f, 0.f, 0.f});

    build_planes();
}

void frustum::on_mousemove(double x, double y) {
    application::on_mousemove(x, y);
}

void frustum::on_mousewheel(double xinc, double yinc) {
    application::on_mousewheel(xinc, yinc);
}

void frustum::update(double t, float dt) {
    clip_to_planes();
    on_resize(sc_width_, sc_height_);
    inc += .01f;
}

void frustum::draw() {
    rndr_state_.clear(0.f, 0.f, 0.f, 1.f);

    auto rot = make_rotation(vec3f(0,1,0), inc);
    camera* cams[4] = { &world_cam_, &lside_cam_, &top_cam_, &rside_cam_};
    for(int i = 0; i != 4; ++i) {
        cams[i]->viewport(cams[i]->viewport());
        prog_tex_.bind();
        prog_tex_.bind_uniform("pvm", cams[i]->proj_view() * rot);
        prog_tex_.release();

        prog_tex_.bind();
        check_tex_.bind(0);
        sampler_.bind(0);
        tex_mesh_.bind();
        tex_mesh_.draw_inst(primitive_type::PT_TRIANGLES, 5*5*5);
        tex_mesh_.release();
        sampler_.release(0);
        check_tex_.release();
        prog_tex_.release();

        rndr_state_.push_state(&plane_state_);
        plane_shdr_.bind();
        plane_shdr_.bind_uniform("mvp", cams[i]->proj_view());
        plane_batch_.bind();
        for(const auto& tok : tokens_) plane_batch_.draw(tok);
        plane_batch_.release();
        plane_shdr_.release();
        rndr_state_.pop();
    }
}

void frustum::shutdown() {
}

int main(int argc, char* argv[]) {
    frustum app;
    app_config conf;
    conf.resizeable_window = true;
    return app.run(conf);
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

void frustum::build_planes() {
    for(auto& tok : tokens_) plane_batch_.free(tok);
    tokens_.clear();
    planes_.clear();

    const auto rmin = world_cam_.get_frustum_plane_points(camera::frustum_plane::FP_RMIN);
    const auto rmax = world_cam_.get_frustum_plane_points(camera::frustum_plane::FP_RMAX);
    const auto umin = world_cam_.get_frustum_plane_points(camera::frustum_plane::FP_UMIN);
    const auto umax = world_cam_.get_frustum_plane_points(camera::frustum_plane::FP_UMAX);
    const auto dmin = world_cam_.get_frustum_plane_points(camera::frustum_plane::FP_DMIN);
    auto dmax = world_cam_.get_frustum_plane_points(camera::frustum_plane::FP_DMAX);

    using geo = geometry3<vtx_p3n3t2_t, primitive_type::PT_TRIANGLE_FAN>;
    auto foo = geo::make_plane(rmin);
    auto tok = plane_batch_.allocate(primitive_type::PT_TRIANGLE_FAN, foo.size(), foo.size());
    std::vector<uint32_t> idx = {0, 1, 2, 3};
    plane_batch_.load(tok, foo, idx);
    tokens_.emplace_back(tok);
    planes_.emplace_back(plane<float>::make_plane(rmin[0], rmin[1], rmin[2]));

    foo = geo::make_plane(rmax);
    tok = plane_batch_.allocate(primitive_type::PT_TRIANGLE_FAN, foo.size(), foo.size());
    plane_batch_.load(tok, foo, idx);
    tokens_.emplace_back(tok);
    planes_.emplace_back(plane<float>::make_plane(rmax[0], rmax[1], rmax[2]));

    foo = geo::make_plane(umin);
    tok = plane_batch_.allocate(primitive_type::PT_TRIANGLE_FAN, foo.size(), foo.size());
    plane_batch_.load(tok, foo, idx);
    tokens_.emplace_back(tok);
    planes_.emplace_back(plane<float>::make_plane(umin[0], umin[1], umin[2]));

    foo = geo::make_plane(umax);
    tok = plane_batch_.allocate(primitive_type::PT_TRIANGLE_FAN, foo.size(), foo.size());
    plane_batch_.load(tok, foo, idx);
    tokens_.emplace_back(tok);
    planes_.emplace_back(plane<float>::make_plane(umax[0], umax[1], umax[2]));

    foo = geo::make_plane(dmin);
    tok = plane_batch_.allocate(primitive_type::PT_TRIANGLE_FAN, foo.size(), foo.size());
    plane_batch_.load(tok, foo, idx);
    tokens_.emplace_back(tok);
    planes_.emplace_back(plane<float>::make_plane(dmin[0], dmin[1], dmin[2]));

    for(auto& p : dmax) p += vec3f{0.f, 0.f, 0.01f};
    foo = geo::make_plane(dmax);
    tok = plane_batch_.allocate(primitive_type::PT_TRIANGLE_FAN, foo.size(), foo.size());
    // Note that foo is on the back clipping plane so we nudge it forward so we can see it
    plane_batch_.load(tok, foo, idx);
    tokens_.emplace_back(tok);
    planes_.emplace_back(plane<float>::make_plane(dmax[0], dmax[1], dmax[2]));
}

void frustum::clip_to_planes() {
    auto rot = make_rotation(vec3f(0,1,0), inc);

    for(auto& s : selected_) s = true;

    for(const auto P : planes_) {
        for(size_t i = 0; i != spheres_.size(); ++i) {
            auto sp = zap::maths::geometry::sphere<float>(rot.transform(spheres_[i]), .2f);
            selected_[i] &= P.distance(sp.centre) > -.2f;
        }
    }

    prog_tex_.bind();
    prog_tex_.bind_uniform("selected", selected_);
    prog_tex_.release();
}

void frustum::on_mousedown(int button) {
    application::on_mousedown(button);

    auto rot = make_rotation(vec3f{0.f, 1.f, 0.f}, inc);

    vec3f O, d;
    if(!world_cam_.pick_ray(mouse_.x, mouse_.y, O, d)) return;

    float parms[2];
    for(size_t i = 0; i != spheres_.size(); ++i) {
        auto sp = zap::maths::geometry::sphere<float>(rot.transform(spheres_[i]), .2f);
        selected_[i] = intersection(sp, zap::maths::geometry::ray3f(O, d), parms) > 0;
    }

    prog_tex_.bind();
    prog_tex_.bind_uniform("selected", selected_);
    prog_tex_.release();
}

void frustum::on_mouseup(int button) {
    application::on_mouseup(button);
    LOG("up:", mouse_);
}
