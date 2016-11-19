/* Created by Darren Otgaar on 2016/08/07. http://www.github.com/otgaard/zap */
#include "quad.hpp"
#include <generators/textures/planar.hpp>
#include <renderer/colour.hpp>

#define GLSL(src) "#version 330 core\n" #src

using namespace zap::graphics2;

const char* const quadvshdr_source = GLSL(
    in vec2 position;
    in vec2 texcoord1;
    uniform mat4 proj_matrix;
    uniform mat4 mv_matrix;
    out vec2 tex;
    void main() {
        tex = texcoord1;
        gl_Position = proj_matrix * mv_matrix * vec4(position, 0, 1);
    }
);

const char* const quadfshdr_source = GLSL(
    in vec2 tex;
    uniform sampler2D diffuse;
    out vec4 frag_colour;
    void main() {
        frag_colour = texture(diffuse, tex);
    }
);

quad::quad() : override_(nullptr), screen_(0,0) {
}

quad::~quad() {
}

void quad::set_texture(texture&& tex) {
    texture_ = std::move(tex);

    if(!program_.is_linked()) return;

    program_.bind();
    program_.bind_texture_unit(program_.uniform_location("diffuse"), 0);
    program_.release();
}

bool quad::initialise() {
    if(mesh_.is_allocated()) return true;

    if(!mesh_.allocate() || !vbuf_.allocate()) {
        LOG("Quad Mesh or Vertex Buffer could not be allocated");
        return false;
    }

    mesh_.set_stream(&vbuf_);
    mesh_.bind(); vbuf_.bind();

    vtx_p2t2_t vertices[4] = {
            { {{-1,-1}}, {{0,0}} },
            { {{+1,-1}}, {{1,0}} },
            { {{+1,+1}}, {{1,1}} },
            { {{-1,+1}}, {{0,1}} }
    };

    vbuf_.initialise(4, vertices);
    mesh_.release();

    program_.add_shader(new shader(shader_type::ST_VERTEX, quadvshdr_source));
    program_.add_shader(new shader(shader_type::ST_FRAGMENT, quadfshdr_source));
    if(!program_.link()) {
        LOG("An error occurred during compilation or linkage of the Quad shader program");
        return false;
    }

    auto proj = make_ortho(-1.f, 1.f, -1.f, 1.f, -1.f, 1.f);
    auto cam = make_frame(vec3f(0,0,-1), vec3f(0,1,0), vec3f(0,0,1));

    program_.bind();
    program_.bind_uniform("proj_matrix", proj);
    program_.bind_uniform("mv_matrix", cam);
    program_.bind_texture_unit(program_.uniform_location("diffuse"), 0);
    program_.release();

    texture tex;
    tex.allocate();
    tex.initialise(8, 8, generators::planar<rgb888_t>::make_checker(8, 8, colour::red8, colour::blue8));
    set_texture(std::move(tex));

    return true;
}

void quad::set_program(program* prog) {
    override_ = prog;
}

void quad::update(double t, float dt) {
}

void quad::draw(bool default_bind) {
    if(!texture_.is_allocated() || screen_.is_zero()) return;

    if(override_) override_->bind();
    else          program_.bind();
    if(default_bind) texture_.bind(0);

    mesh_.bind();
    mesh_.draw();
    mesh_.release();

    if(default_bind) texture_.release();
    if(override_) override_->release();
    else          program_.release();
}
