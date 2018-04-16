/* Created by Darren Otgaar on 2016/08/07. http://www.github.com/otgaard/zap */
#include <graphics/graphics2/quad.hpp>
#include <graphics/generators/textures/planar.hpp>
#include <graphics/colour.hpp>
#include <engine/gl_api.hpp>

#define GLSL(src) "#version 330 core\n" #src

using namespace zap::graphics;

const char* const quadvshdr_source = GLSL(
    in vec2 position;
    in vec2 texcoord1;
    out vec2 tex;
    void main() {
        tex = texcoord1;
        gl_Position = vec4(position, 0, 1);
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

quad::quad() : frag_shdr_(nullptr), screen_(0,0) {
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

bool quad::initialise(shader* frag_shdr) {
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

    gl_error_check();

    frag_shdr_ = frag_shdr;
    program_.add_shader(new shader(shader_type::ST_VERTEX, quadvshdr_source));
    program_.add_shader(frag_shdr_ ? frag_shdr_ : new shader(shader_type::ST_FRAGMENT, quadfshdr_source));
    if(!program_.link()) {
        LOG_ERR("An error occurred during compilation or linkage of the Quad shader program");
        return false;
    }

    program_.bind();

    if(!frag_shdr) {
        program_.bind_texture_unit(program_.uniform_location("diffuse"), 0);

        texture tex;
        tex.allocate();
        tex.initialise(8, 8, generators::planar<rgb888_t>::make_checker(8, 8, colour::red8, colour::blue8));
        set_texture(std::move(tex));
    }

    program_.release();

    return true;
}

void quad::resize(int w, int h) {
    screen_.set(w, h);
}

void quad::update(double t, float dt) {
}

void quad::draw() {
    if(screen_.is_zero()) return;

    program_.bind();
    if(frag_shdr_ == nullptr && !tex_override_) texture_.bind(0);
    else if(tex_override_) tex_override_->bind(0);

    mesh_.bind();
    gl::glGetIntegerv(GL_VIEWPORT, curr_viewport_);
    gl::glViewport(0, 0, screen_.x, screen_.y);
    gl_error_check();
    mesh_.draw(engine::primitive_type::PT_TRIANGLE_FAN);
    gl_error_check();
    gl::glViewport(curr_viewport_[0], curr_viewport_[1], curr_viewport_[2], curr_viewport_[3]);
    mesh_.release();

    if(frag_shdr_ == nullptr && !tex_override_) texture_.release();
    else if(tex_override_) tex_override_->release();
    program_.release();
    gl_error_check();
}

bool quad::initialise(const std::string& frag_shdr_src) {
    return initialise(new shader(shader_type::ST_FRAGMENT, frag_shdr_src));
}
