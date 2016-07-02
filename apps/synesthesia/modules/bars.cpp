/* Created by Darren Otgaar on 2016/06/10. http://www.github.com/otgaard/zap */
#include "bars.hpp"
#include "apps/graphic_types.hpp"

#include <generators/textures/planar.hpp>

using namespace zap;
using namespace zap::maths;
using namespace zap::engine;

const char* bars_vshader = GLSL(
    uniform mat4 proj_matrix;

    in vec2 position;
    in vec2 texcoord1;

    out vec2 texcoord;

    void main() {
        texcoord = texcoord1;
        gl_Position = proj_matrix * vec4(position.xy, 0, 1);
    }
);

const char* bars_fshader = GLSL(
    out vec4 frag_colour;
    void main() {
        frag_colour = vec4(1);
    }
);

const char* bars_ftexshader = GLSL(
    in vec2 texcoord;
    out vec4 frag_colour;
    uniform sampler2D tex;

    void main() {
        frag_colour = texture(tex, texcoord);
    }
);

using bars_vbuf_t = vertex_buffer<vtx_p2t2_t, buffer_usage::BU_DYNAMIC_DRAW>;
using bars_mesh_t = mesh<vertex_stream<bars_vbuf_t>, primitive_type::PT_TRIANGLES>;

struct bars::state_t {
    program bars_program;
    bars_vbuf_t bars_buffer;
    bars_mesh_t bars_mesh;
    texture checker;
};

bars::bars(application* app_ptr) : module(app_ptr, "bars"), state(new state_t()) {
    UNUSED(bars_fshader);

    // The OpenGL Context should be valid here
    auto& s = *state.get();

    s.bars_program.add_shader(new shader(shader_type::ST_VERTEX, bars_vshader));
    s.bars_program.add_shader(new shader(shader_type::ST_FRAGMENT, bars_ftexshader));
    if(!s.bars_program.link(true)) gl_error_check();

    s.checker.allocate();
    //s.checker.initialise(1, 2, generators::planar<rgb888_t>::make_checker(1, 2, colour::red8, colour::blue8), false);
    s.checker.initialise(1, 256, generators::planar<rgb888_t>::make_lerp(256, colour::red8, colour::yellow8), false);
    s.checker.bind();

    s.bars_program.bind();
    auto loc = s.bars_program.uniform_location("tex");
    s.bars_program.bind_texture_unit(loc, 0);

    s.bars_mesh.set_stream(&s.bars_buffer);

    s.bars_mesh.allocate();
    s.bars_mesh.bind();
    s.bars_buffer.allocate();
    s.bars_buffer.bind();

    mat4f proj_matrix = {
            60/1280.f,       0.f, 0.f, 0.f,
            0.f,        60/768.f, 0.f, 0.f,
            0.f,             0.f, 2.f, 0.f,
            0.f,             0.f, 0.f, 1.f
    };

    s.bars_program.bind();
    loc = s.bars_program.uniform_location("proj_matrix");
    s.bars_program.bind_uniform(loc, proj_matrix);
    s.bars_buffer.initialise(2*3*64);     // 2 tris per bar, 64 bars in total

    const float x_off = -20.f;
    const float inv_x = 40/64.f;

    if(s.bars_buffer.map(buffer_access::BA_WRITE_ONLY)) {
        for(size_t i = 0; i != 64; ++i) {
            size_t idx = 6*i;
            float dx = x_off + i*inv_x;
            s.bars_buffer[idx].position = vec2f(dx, -10.f);         s.bars_buffer[idx++].texcoord1 = vec2f(0, 0);
            s.bars_buffer[idx].position = vec2f(dx + inv_x, -10.f); s.bars_buffer[idx++].texcoord1 = vec2f(1, 0);
            s.bars_buffer[idx].position = vec2f(dx + inv_x, +10.f); s.bars_buffer[idx++].texcoord1 = vec2f(1, 1);
            s.bars_buffer[idx].position = vec2f(dx, -10.f);         s.bars_buffer[idx++].texcoord1 = vec2f(0, 0);
            s.bars_buffer[idx].position = vec2f(dx + inv_x, +10.f); s.bars_buffer[idx++].texcoord1 = vec2f(1, 1);
            s.bars_buffer[idx].position = vec2f(dx, +10.f);         s.bars_buffer[idx].texcoord1 = vec2f(0, 1);
        }
    }
    s.bars_buffer.unmap();
    s.bars_mesh.release();
}

bars::~bars() {
}

void bars::update(double t, float dt) {
    auto& s = *state.get();
    s.bars_buffer.bind();
    if(s.bars_buffer.map(buffer_access::BA_WRITE_ONLY)) {
        for(size_t i = 0; i != 64; ++i) {
            size_t idx = 6*i;
            float height = -10.f + 20.f*window_[i];
            s.bars_buffer[idx+2].position.y = height;
            s.bars_buffer[idx+4].position.y = height;
            s.bars_buffer[idx+5].position.y = height;
        }
    }
    s.bars_buffer.unmap();
    s.bars_buffer.release();
}

void bars::draw() {
    auto& s = *state.get();
    s.bars_program.bind();
    s.checker.bind();
    s.bars_mesh.bind();
    s.bars_mesh.draw();
    s.bars_mesh.release();
    s.checker.release();
    s.bars_program.release();
}

