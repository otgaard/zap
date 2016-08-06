/* Created by Darren Otgaar on 2016/07/06. http://www.github.com/otgaard/zap */
#include <maths/rand_lcg.hpp>
#include "particles.hpp"
#include "apps/graphic_types.hpp"

#include <maths/curves/hermite.hpp>
#include <algorithm>

using namespace zap;
using namespace zap::maths;
using namespace zap::engine;

const char* particles_vshader = GLSL(
    uniform mat4 proj_matrix;
    uniform mat4 mv_matrix;

    in vec3 position;
    in vec4 colour1;
    in float pointsize;

    out vec4 col1;

    void main() {
        col1 = colour1;
        gl_PointSize = pointsize*0.02*(100. - length(position));
        gl_Position = proj_matrix * mv_matrix * vec4(position, 1.0);
    }
);

const char* particles_fshader = GLSL(
    in vec4 col1;
    out vec4 frag_colour;
    void main() {
        frag_colour = col1;
    }
);

using particles_vbuf_t = vertex_buffer<vtx_p3c4ps1_t, buffer_usage::BU_DYNAMIC_DRAW>;
using particles_mesh_t = mesh<vertex_stream<particles_vbuf_t>, primitive_type::PT_POINTS>;

struct particles::state_t {
    program particles_program;
    particles_vbuf_t particles_buffer;
    particles_mesh_t particles_mesh;
    curves::hermite<float, vec3f> curve;
};

particles::particles(application* app_ptr) : module(app_ptr, "particles"), state(new state_t()) {
    auto& s = *state.get();

    s.particles_program.add_shader(new shader(shader_type::ST_VERTEX, particles_vshader));
    s.particles_program.add_shader(new shader(shader_type::ST_FRAGMENT, particles_fshader));
    if(!s.particles_program.link(true)) gl_error_check();

    s.particles_program.bind();
    s.particles_program.bind_uniform("proj_matrix", make_perspective(45.f, 1280.f/768.f, .5f, 100.f));
    s.particles_program.bind_uniform("mv_matrix", make_translation(0.f, 0.f, 0.f));

    if(!s.particles_buffer.allocate() || !s.particles_mesh.allocate()) {
        LOG_ERR("Couldn't allocate buffer or mesh for particles");
    }

    s.particles_mesh.bind();
    s.particles_buffer.bind();
    s.particles_mesh.set_stream(&s.particles_buffer);
    s.particles_buffer.initialise(50000);    // Simulate on CPU for now

    rand_lcg rand;

    if(s.particles_buffer.map(buffer_access::BA_WRITE_ONLY)) {
        std::for_each(s.particles_buffer.begin(), s.particles_buffer.end(), [rand](vtx_p3c4ps1_t& vtx) {
            vtx.position = vec3f(rand.random_s(), rand.random_s(), -100.f*rand.random());
            vtx.colour1 = lerp(rand.random(), vec4f(0.5f,0.5f,1,1), vec4f(1,1,1,1));
            vtx.pointsize = 2.f*rand.random();
        });
        s.particles_buffer.unmap();
    }
    s.particles_mesh.release();

    s.curve.set_points(vec3f(0,0,0), vec3f(1,1,1));
    s.curve.set_tangents(vec3f(1,0,0), vec3f(1,0,0));
}

particles::~particles() {
}

static float rotation = 0.f;

void particles::update(double t, float dt) {
    static rand_lcg generator;
    auto& s = *state.get();
    s.particles_buffer.bind();
    if(s.particles_buffer.map(buffer_access::BA_READ_WRITE)) {
        auto rand = &generator; const auto& scale = analysis_.estimated_beat;
        auto rot_v = vec3f(std::cos(rotation), std::sin(rotation), 0);
        auto radius = 0.5f * (1.f - scale) * 75.f;
        std::for_each(s.particles_buffer.begin(), s.particles_buffer.end(), [dt, rand, scale, rot_v, radius](vtx_p3c4ps1_t& vtx) {
            if(vtx.position.z > 0) {
                float angle = TWO_PI*rand->random();
                if(rand->random() > 0.5f) {
                    auto r = radius*vec2f(std::cos(angle), std::sin(angle)) * rand->random();
                    vtx.position = vec3f(r.x, r.y, -100 + -5 * rand->random());
                } else {
                    auto v = rot_v*rand->random_s();
                    vtx.position = vec3f(v.x, v.y, -100 + -5 * rand->random());
                }
                vtx.colour1 = lerp(scale, vec4f(0.5f, 0.5f, 1,1), vec4f(1.f,.6f,0.f,1.f));
            }
            vtx.position += dt*vec3f(0,0,(clamp(scale-.3f)*100) + 10);
        });
        s.particles_buffer.unmap();
    }
    s.particles_buffer.release();

    rotation = wrap<float>(rotation + 0.1f*analysis_.estimated_beat, -TWO_PI, TWO_PI);
}

void particles::draw() {
    auto& s = *state.get();
    s.particles_program.bind();
    s.particles_mesh.bind();
    s.particles_mesh.draw();
    s.particles_mesh.release();
    s.particles_program.release();
}
