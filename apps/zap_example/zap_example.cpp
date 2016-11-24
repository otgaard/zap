/* Created by Darren Otgaar on 2016/03/05. http://www.github.com/otgaard/zap */
#include <map>

#define LOGGING_ENABLED
#include <maths/io.hpp>
#include <tools/log.hpp>
#include <maths/geometry/AABB.hpp>
#include <generators/textures/planar.hpp>
#include "apps/application.hpp"
#include "apps/graphic_types.hpp"
#include "generators/geometry/geometry3.hpp"

using namespace zap;
using namespace zap::maths;
using namespace zap::generators;
using namespace zap::maths::geometry;

using ico_vertex_t = vertex<pos3f_t>;
using ico_vbuf_t = vertex_buffer<ico_vertex_t, buffer_usage::BU_STATIC_DRAW>;
using u16_ibuf_t = index_buffer<uint16_t, primitive_type::PT_TRIANGLES, buffer_usage::BU_STATIC_DRAW>;
using ico_mesh_t = mesh<vertex_stream<ico_vbuf_t>, primitive_type::PT_TRIANGLES, u16_ibuf_t>;

using tex_vertex_t = vertex<pos3f_t, nor3f_t, tex2f_t>;
using tex_vbuf_t = vertex_buffer<tex_vertex_t, buffer_usage::BU_STATIC_DRAW>;
using tex_mesh_t = mesh<vertex_stream<tex_vbuf_t>, primitive_type::PT_TRIANGLES, u16_ibuf_t>;

class zap_example : public application {
public:
    zap_example() : application("zap_example", 1280, 768, false) { }

    bool initialise() override final;
    void update(double t, float dt) override final;
    void draw() override final;
    void shutdown() override final;

    void on_resize(int width, int height) override final;
    void on_mousemove(double x, double y) override final;
    void on_mousewheel(double xinc, double yinc) override final;

protected:
    void make_isosphere(int subdivision_levels);
    void make_textured_sphere();

    bool initialise_buffers();
    bool initialise_programs();

    ico_vbuf_t ico_vb_;
    u16_ibuf_t ico_ib_;
    ico_mesh_t ico_mesh_;

    tex_vbuf_t tex_vb_;
    u16_ibuf_t tex_ib_;
    tex_mesh_t tex_mesh_;

    texture check_tex_;

    program prog_ico_;
    program prog_tex_;

    camera cam_;
};

const char* const ico_vtx_shdr = GLSL(
    uniform mat4 pvm;
    in vec3 position;
    out vec3 normal;
    void main() {
        normal = position;
        gl_Position = pvm*vec4(position,1);
    }
);

const char* const ico_frg_shdr = GLSL(
    const vec3 ld = vec3(0,0.707,0.707);
    in vec3 normal;
    out vec4 frag_colour;
    void main() {
        float s = max(dot(normal,ld),0) + .2;
        frag_colour = vec4(s, s, s, 1);
    }
);

const char* const tex_vtx_shdr = GLSL(
    uniform mat4 pvm;

    in vec3 position;
    in vec3 normal;
    in vec2 texcoord1;

    out vec3 nor;
    out vec2 tex;

    void main() {
        nor = normal;
        tex = texcoord1;
        gl_Position = pvm * vec4(position, 1);
    }
);

const char* const tex_frg_shdr = GLSL(
    const vec3 ld = vec3(0, 0.707, 0.707);

    uniform sampler2D diffuse_tex;

    in vec3 nor;
    in vec2 tex;

    out vec4 frag_colour;

    void main() {
        float s = max(dot(nor,ld),0) + .2;
        frag_colour = vec4(s * texture(diffuse_tex, tex).rgb, 1);
    }
);

bool zap_example::initialise_programs() {
    prog_ico_.add_shader(shader_type::ST_VERTEX, ico_vtx_shdr);
    prog_ico_.add_shader(shader_type::ST_FRAGMENT, ico_frg_shdr);
    if(!prog_ico_.link(true)) {
        LOG_ERR("Error linking ico shader");
        return false;
    }

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

bool zap_example::initialise_buffers() {
    if(!ico_vb_.allocate() || !ico_ib_.allocate() || !ico_mesh_.allocate()) {
        LOG_ERR("Could not allocate icosphere mesh resources");
        return false;
    }

    ico_mesh_.set_stream(&ico_vb_); ico_mesh_.set_index(&ico_ib_);
    make_isosphere(3);

    if(!tex_vb_.allocate() || !tex_ib_.allocate() || !tex_mesh_.allocate()) {
        LOG_ERR("Could not allocate textured mesh resources");
        return false;
    }

    tex_mesh_.set_stream(&tex_vb_); tex_mesh_.set_index(&tex_ib_);
    make_textured_sphere();
    return true;
}

bool zap_example::initialise() {
    cam_.viewport(0, 0, 1280, 768);
    cam_.frustum(45.f, 1280.f/768.f, .5f, 10.f);
    cam_.world_pos(vec3f(0,0,5));
    cam_.orthogonolise(vec3f(0,0,-1));

    if(!initialise_programs()) {
        LOG_ERR("Failed to initialise program resources for example");
        return false;
    }

    if(!initialise_buffers()) {
        LOG_ERR("Failed to initialise buffer resources for example");
        return false;
    }

    auto pixels = planar<rgb888_t>::make_checker(8, 8, colour::blue8, colour::green8);
    check_tex_.allocate();
    check_tex_.initialise(8, 8, pixels, false);

    return true;
}

void zap_example::on_resize(int width, int height) {
}

void zap_example::on_mousemove(double x, double y) {
}

void zap_example::on_mousewheel(double xinc, double yinc) {
}

float inc = 0;

void zap_example::update(double t, float dt) {
    auto rot = make_rotation(vec3f(0,1,0), inc);

    prog_ico_.bind();
    prog_ico_.bind_uniform("pvm", cam_.proj_view() * rot * make_scale<float>(.5f, .5f, .5f));
    prog_ico_.release();

    prog_tex_.bind();
    prog_tex_.bind_uniform("pvm", cam_.proj_view() * rot * make_translation<float>(0, 1, 0));
    prog_tex_.release();

    inc += 0.01f;
}

void zap_example::draw() {
    depth_test(true);

    prog_ico_.bind();
    ico_mesh_.bind();
    ico_mesh_.draw();
    ico_mesh_.release();
    prog_ico_.release();

    prog_tex_.bind();
    check_tex_.bind(0);
    tex_mesh_.bind();
    tex_mesh_.draw();
    tex_mesh_.release();
    check_tex_.release();
    prog_tex_.release();

    depth_test(false);
}

void zap_example::shutdown() {
}

int main(int argc, char* argv[]) {
    zap_example app;
    return app.run();
}

void zap_example::make_isosphere(int subdivision_levels) {
    std::vector<ico_vertex_t> vbuf;
    std::vector<unsigned short> ibuf;
    std::tie(vbuf, ibuf) = geometry3<ico_vertex_t, primitive_type::PT_TRIANGLES>::make_isosphere(subdivision_levels);
    ico_mesh_.bind();
    ico_vb_.bind();
    ico_vb_.initialise(vbuf);
    ico_ib_.bind();
    ico_ib_.initialise(ibuf);
    ico_mesh_.release();
}

void zap_example::make_textured_sphere() {
    std::vector<tex_vertex_t> vbuf;
    std::vector<unsigned short> ibuf;
    std::tie(vbuf, ibuf) = geometry3<tex_vertex_t, primitive_type::PT_TRIANGLES>::make_UVsphere(30, 60, .5f, false);
    tex_mesh_.bind();
    tex_vb_.bind();
    tex_vb_.initialise(vbuf);
    tex_ib_.bind();
    tex_ib_.initialise(ibuf);
    tex_mesh_.release();
}
