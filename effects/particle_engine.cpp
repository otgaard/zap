/* Created by Darren Otgaar on 2016/10/28. http://www.github.com/otgaard/zap */
#include "particle_engine.hpp"
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

using pos3_t = core::position<vec3f>;   // Position of particle
using vel3_t = core::normal<vec3f>;     // Velocity of particle
using mae3_t = core::tangent<vec3f>;    // Mass, Age, Extra

using particle_pos_vtx_t = engine::vertex<pos3_t>;
using particle_vel_vtx_t = engine::vertex<vel3_t>;
using particle_mae_vtx_t = engine::vertex<mae3_t>;

using particle_pos_vbuf_t = engine::vertex_buffer<particle_pos_vtx_t, buffer_usage::BU_STREAM_DRAW>;
using particle_vel_vbuf_t = engine::vertex_buffer<particle_vel_vtx_t, buffer_usage::BU_STREAM_DRAW>;
using particle_mae_vbuf_t = engine::vertex_buffer<particle_mae_vtx_t, buffer_usage::BU_STREAM_DRAW>;

using particle_stream_t = vertex_stream<particle_pos_vbuf_t, particle_vel_vbuf_t, particle_mae_vbuf_t>;

using particle_mesh_t = engine::mesh<particle_stream_t, primitive_type::PT_POINTS>;

struct particle_engine::state_t {
    size_t allocated;                       // Number of active particles in simulation
    framebuffer output_buffer;              // 3 targets (pos, vel, mae)
    particle_pos_vbuf_t position_vbuf;
    particle_vel_vbuf_t velocity_vbuf;
    particle_mae_vbuf_t mass_age_vbuf;
    particle_mesh_t particle_mesh;
};

particle_engine::particle_engine() : state_(new state_t()), s(*state_.get()) {
}

particle_engine::~particle_engine() {
}

bool particle_engine::initialise() {
    // Start off with just 100000 particles

    const size_t particle_count = 100000;

    s.allocated = 0;

    if(!s.output_buffer.initialise(3, particle_count, 1, pixel_format::PF_RGB, pixel_datatype::PD_FLOAT, false, false)) {
        LOG_ERR("Error initialising Framebuffer");
        return false;
    }

    if(!s.position_vbuf.allocate() || !s.velocity_vbuf.allocate() || !s.mass_age_vbuf.allocate() || !s.particle_mesh.allocate()) {
        LOG_ERR("Failure allocating particle engine buffer resources");
        s.position_vbuf.deallocate(); s.velocity_vbuf.deallocate(); s.mass_age_vbuf.deallocate();
        return false;
    }

    s.particle_mesh.set_stream(particle_stream_t(&s.position_vbuf, &s.velocity_vbuf, &s.mass_age_vbuf));

    s.particle_mesh.bind(); s.position_vbuf.bind(); s.velocity_vbuf.bind(); s.mass_age_vbuf.bind();

    s.position_vbuf.initialise(particle_count);
    s.velocity_vbuf.initialise(particle_count);
    s.mass_age_vbuf.initialise(particle_count);

    s.particle_mesh.release();

    return true;
}

void particle_engine::update(double t, float dt) {

}

void particle_engine::draw(const renderer::camera& cam) {

}
