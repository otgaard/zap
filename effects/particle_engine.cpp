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

};

particle_engine::particle_engine() : state_(new state_t()), s(*state_.get()) {
}

particle_engine::~particle_engine() {
}

bool particle_engine::initialise() {

}

void particle_engine::update(double t, float dt) {

}

void particle_engine::draw(const renderer::camera& cam) {

}
