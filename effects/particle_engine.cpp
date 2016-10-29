/* Created by Darren Otgaar on 2016/10/28. http://www.github.com/otgaard/zap */
#include <apps/graphic_types.hpp>
#include <maths/rand_lcg.hpp>
#include "part_engine.hpp"
#include "engine/framebuffer.hpp"
#include "engine/vertex_buffer.hpp"
#include "engine/mesh.hpp"
#include "engine/texture.hpp"
#include "renderer/camera.hpp"
#include "maths/algebra.hpp"

using namespace zap;
using namespace zap::maths;
using namespace zap::engine;
using namespace zap::effects;

// We need a "quad" of dims [particle_count,1] for rendering

using quad_vertex_t = vertex<pos2f_t>;
using quad_vbuf_t = vertex_buffer<quad_vertex_t, buffer_usage::BU_STATIC_DRAW>;
using quad_mesh_t = mesh<vertex_stream<quad_vbuf_t>, primitive_type::PT_TRIANGLE_FAN>;

using vec3_pbuf_t = pixel_buffer<rgb32f_t, buffer_usage::BU_STREAM_COPY>;

using particle_vbuf_t = vertex_buffer<pos3f_t, buffer_usage::BU_STREAM_COPY>;
using particle_mesh_t = mesh<vertex_stream<particle_vbuf_t>, primitive_type::PT_POINTS>;

extern const char* const particle_sim_vshdr;
extern const char* const particle_sim_fshdr;

extern const char* const particle_rndr_vshdr;
extern const char* const particle_rndr_fshdr;

struct particle_engine::state_t {
    const size_t particle_count = 100000;
    size_t dim;

    quad_vbuf_t quad_vbuf;
    quad_mesh_t quad_mesh;

    particle_mesh_t particle_mesh;          // Only need the mesh.  Poor design.  Adapter for pixel buffers? Any buffer?

    size_t allocated;                       // Number of active particles in simulation

    framebuffer buffers[2];                 // 3 targets (pos, vel, mae)
    int active_buffer;                      // Buffer switch

    vec3_pbuf_t copy_buffer;                // Used, for now, for copying data from the framebuffers and rendering the particles
                                            // Should be replaced by buffer textures to remove the copy so position_tex can be
                                            // bound directly as vertices in render pass (point sprites)

    program sim_pass;                       // Simulation Shader Pass
    program rndr_pass;                      // Render Shader Pass
};

particle_engine::particle_engine() : state_(new state_t()), s(*state_.get()) {
}

particle_engine::~particle_engine() {
}

bool particle_engine::initialise() {
    s.allocated = 0;

    s.dim = (size_t)pow(2, std::ceil(log2(sqrt(s.particle_count))));
    LOG("dim", s.dim, s.dim*s.dim, s.particle_count);

    for(auto& buf : s.buffers) {

        if(!buf.allocate() || !buf.initialise(3, s.dim, s.dim, pixel_format::PF_RGB, pixel_datatype::PD_FLOAT, false, false)) {
            LOG_ERR("Failure allocating/initialising framebuffers");
            return false;
        }
    }

    std::vector<rgb32f_t> initial_conditions;
    initial_conditions.reserve(s.dim*s.dim);
    rand_lcg rand;

    for(int i = 0; i != s.dim*s.dim; ++i) {
        initial_conditions.push_back(rgb32f_t(rand.random_s(), rand.random_s(), rand.random_s()));
    }

    if(!s.copy_buffer.allocate()) {
        LOG_ERR("Failed to allocate pixel copy buffer");
        return false;
    }

    s.copy_buffer.bind();
    if(!s.copy_buffer.initialise(s.dim, s.dim, initial_conditions.data())) {
        LOG_ERR("Failed to initialise pixel copy buffer");
        return false;
    }
    s.copy_buffer.release();

    if(!s.quad_vbuf.allocate() || !s.quad_mesh.allocate()) {
        LOG_ERR("Failed to initialise screen quad");
        return false;
    }

    s.quad_mesh.set_stream(&s.quad_vbuf);
    s.quad_mesh.bind(); s.quad_vbuf.bind();

    std::vector<quad_vertex_t> quad_vertices = {{
            {{ vec2f(0.f,              0.f) }},
            {{ vec2f(s.particle_count, 0.f) }},
            {{ vec2f(s.particle_count, 1.f) }},
            {{ vec2f(0.f,              1.f) }}
    }};

    s.quad_vbuf.initialise(quad_vertices);

    s.quad_mesh.release();

    if(!s.particle_mesh.allocate()) {
        LOG_ERR("Failed to initialise particle mesh");
        return false;
    }

    return true;
}

void particle_engine::update(double t, float dt) {

}

void particle_engine::draw(const renderer::camera& cam) {

}

const char* const particle_sim_vshdr = GLSL(

);

const char* const particle_sim_fshdr = GLSL(

);

const char* const particle_rndr_vshdr = GLSL(

);

const char* const particle_rndr_fshdr = GLSL(

);
