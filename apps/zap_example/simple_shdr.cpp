/* Created by Darren Otgaar on 2016/07/31. http://www.github.com/otgaard/zap */
#include "simple_shdr.hpp"
#include "apps/graphic_types.hpp"

using namespace zap;
using namespace engine;

const char* const vtx_shdr = GLSL(
    in vec3 position;
    uniform mat4 pvm_matrix;
    void main() {
        gl_Position = pvm_matrix * vec4(position,1);
    }
);

const char* const frg_shdr = GLSL(
    out vec4 frag_colour;
    void main() {
        frag_colour = vec4(1,1,1,1);
    }
);

simple_shdr::simple_shdr() : pvm_loc_(-1) {
}

simple_shdr::~simple_shdr() {
}

bool simple_shdr::initialise() {
    add_shader(shader_type::ST_VERTEX, vtx_shdr);
    add_shader(shader_type::ST_FRAGMENT, frg_shdr);
    if(!link()) {
        LOG_ERR("Error creating simple_shdr class");
        return false;
    }

    pvm_loc_ = uniform_location("pvm_matrix");
    LOG("pvm_loc", pvm_loc_);
    return true;
}
