/* Created by Darren Otgaar on 2016/07/24. http://www.github.com/otgaard/zap */
#include "quad.hpp"
#include "apps/graphic_types.hpp"
#include <generators/textures/planar.hpp>

const char* const vtxshdr_source = GLSL(
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

const char* const frgshdr_source = GLSL(
    in vec2 tex;
    uniform sampler2D diffuse;
    out vec4 frag_colour;
    void main() {
        frag_colour = texture(diffuse, tex);
    }
);

quad::quad() {
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

    program_.add_shader(new shader(shader_type::ST_VERTEX, vtxshdr_source));
    program_.add_shader(new shader(shader_type::ST_FRAGMENT, frgshdr_source));
    if(!program_.link()) {
        LOG("An error occurred during compilation or linkage of the Quad shader program");
        return false;
    }

    program_.bind();
    program_.bind_texture_unit(program_.uniform_location("diffuse"), 0);
    program_.release();

    return true;
}

void quad::update(double t, float dt) {
}

void quad::draw(const camera& cam) {
    if(!texture_.is_allocated()) return;

    program_.bind();
    program_.bind_uniform("proj_matrix", cam.projection());
    program_.bind_uniform("mv_matrix", cam.world_to_view() * world_transform.gl_matrix());
    texture_.bind(0);
    mesh_.bind();
    mesh_.draw();
    mesh_.release();
    texture_.release();
    program_.release();
}
