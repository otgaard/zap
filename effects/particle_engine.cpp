/* Created by Darren Otgaar on 2016/10/28. http://www.github.com/otgaard/zap */

//#include <examples/graphic_types.hpp>
#include <graphics3/g3_types.hpp>
#include <maths/rand_lcg.hpp>
#include "particle_engine.hpp"
#include <maths/io.hpp>
#include <engine/pixel_format.hpp>
#include <engine/pixel_buffer.hpp>
#include <engine/program.hpp>
#include <engine/framebuffer.hpp>
#include <renderer/camera.hpp>

using namespace zap;
using namespace zap::maths;
using namespace zap::engine;
using namespace zap::effects;
using namespace zap::graphics;

using quad_vertex_t = vertex<pos2f_t, tex2f_t>;
using quad_vbuf_t = vertex_buffer<quad_vertex_t>;
using quad_mesh_t = mesh<vertex_stream<quad_vbuf_t>, primitive_type::PT_TRIANGLE_FAN>;

using vec3_pbuf_t = pixel_buffer<rgb32f_t>;

using particle_vbuf_t = vertex_buffer<vertex<pos3f_t>>;     // BU_STREAM_COPY
using particle_mesh_t = mesh<vertex_stream<particle_vbuf_t>, primitive_type::PT_POINTS>;

extern const char* const particle_sim_vshdr;
extern const char* const particle_sim_fshdr;

extern const char* const particle_rndr_vshdr;
extern const char* const particle_rndr_fshdr;

struct particle_engine::state_t {
    const size_t particle_count = 250000;   // 1/4 million to start
    size_t dim;

    quad_vbuf_t quad_vbuf;
    quad_mesh_t quad_mesh;

    particle_vbuf_t particle_vbuf;          // Attributes...
    particle_mesh_t particle_mesh;          // Only need the mesh.  Poor design.  Adapter for pixel buffers? Any buffer?

    size_t allocated;                       // Number of active particles in simulation

    framebuffer buffers[2];                 // 3 targets (pos, vel, mae)
    int active_buffer;                      // Buffer switch

    vec3_pbuf_t copy_buffer;                // Used, for now, for copying data from the framebuffers and rendering the particles
                                            // Should be replaced by buffer textures to remove the copy so position_tex can be
                                            // bound directly as vertices in render pass (point sprites)

    program sim_pass;                       // Simulation Shader Pass
    program rndr_pass;                      // Render Shader Pass

    // Temporary - Just for testing various concepts
    float angle;
};

particle_engine::particle_engine() : state_(new state_t()), s(*state_.get()) {
}

particle_engine::~particle_engine() {
}

bool particle_engine::setup_buffers() {
    for(auto& buf : s.buffers) {
        if(!buf.allocate() || !buf.initialise(3, s.dim, s.dim, pixel_format::PF_RGB, pixel_datatype::PD_FLOAT, false, false)) {
            LOG_ERR("Failure allocating/initialising framebuffers");
            return false;
        }
    }

    std::vector<rgb32f_t> initial_position;
    initial_position.reserve(s.dim*s.dim);
    rand_lcg rand;

    for(int i = 0, end = s.dim*s.dim; i != end; ++i) {
        auto P = vec3f(rand.random_s(), rand.random_s(), rand.random_s());
        while(P.length_sqr() > 1.) P.set(rand.random_s(), rand.random_s(), rand.random_s());

        initial_position.push_back(rgb32f_t(P.x, P.y, P.z));
    }

    if(!s.copy_buffer.allocate()) {
        LOG_ERR("Failed to allocate pixel copy buffer");
        return false;
    }

    s.copy_buffer.bind();
    if(!s.copy_buffer.initialise(s.dim, s.dim, initial_position.data())) {
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
                                                        { { vec2f(-1.f, -1.f) }, { vec2f(0.f, 0.f) } },
                                                        { { vec2f(+1.f, -1.f) }, { vec2f(1.f, 0.f) } },
                                                        { { vec2f(+1.f, +1.f) }, { vec2f(1.f, 1.f) } },
                                                        { { vec2f(-1.f, +1.f) }, { vec2f(0.f, 1.f) } }
                                                }};

    s.quad_vbuf.initialise(quad_vertices);

    s.quad_mesh.release();

    if(!s.particle_vbuf.allocate() || !s.particle_mesh.allocate()) {
        LOG_ERR("Failed to initialise particle mesh");
        return false;
    }

    s.copy_buffer.bind();

    s.particle_mesh.set_stream(&s.particle_vbuf);
    s.particle_mesh.bind();
    s.particle_vbuf.bind();
    s.particle_vbuf.initialise(s.particle_count);
    s.particle_mesh.release();

    if(!s.buffers[0].write_attachment(s.copy_buffer, vec4i(0, 0, s.dim, s.dim), 0)) {
        LOG_ERR("Error during initialisation of particle system from pixel buffer");
        return false;
    }

    if(!s.buffers[0].write_attachment(s.copy_buffer, vec4i(0, 0, s.dim, s.dim), 1)) {
        LOG_ERR("Error during initialisation of particle system from pixel buffer");
        return false;
    }

    if(!s.buffers[0].write_attachment(s.copy_buffer, vec4i(0, 0, s.dim, s.dim), 2)) {
        LOG_ERR("Error during initialisation of particle system from pixel buffer");
        return false;
    }

    s.copy_buffer.release();
    s.active_buffer = 0;
    return true;
}

bool particle_engine::setup_shaders() {
    s.sim_pass.add_shader(shader_type::ST_VERTEX, particle_sim_vshdr);
    s.sim_pass.add_shader(shader_type::ST_FRAGMENT, particle_sim_fshdr);
    if(!s.sim_pass.link()) {
        LOG_ERR("Particle Simulation Pass shader failed to compile");
        return false;
    }

    s.rndr_pass.add_shader(shader_type::ST_VERTEX, particle_rndr_vshdr);
    s.rndr_pass.add_shader(shader_type::ST_FRAGMENT, particle_rndr_fshdr);
    if(!s.rndr_pass.link()) {
        LOG_ERR("Particle Render Pass shader failed to compile");
        return false;
    }

    // Set up shaders
    s.sim_pass.bind();
    s.sim_pass.bind_texture_unit(s.sim_pass.uniform_location("position_tex"), 0);
    s.sim_pass.bind_texture_unit(s.sim_pass.uniform_location("velocity_tex"), 1);
    s.sim_pass.bind_texture_unit(s.sim_pass.uniform_location("colour_tex"), 2);
    s.sim_pass.release();

    s.rndr_pass.bind();
    s.rndr_pass.bind_texture_unit(s.rndr_pass.uniform_location("colour_tex"), 0);
    s.rndr_pass.bind_uniform("dim", int(s.dim));
    s.rndr_pass.release();

    return true;
}

bool particle_engine::initialise() {
    s.allocated = 0;
    s.dim = (size_t)std::pow(2, std::ceil(std::log2(std::sqrt(s.particle_count))));
    LOG("PARTICLE ENGINE DIMENSIONS:", s.dim, s.dim*s.dim, s.particle_count);

    if(!setup_buffers() || !setup_shaders()) {
        LOG_ERR("Failed to setup Particle Engine");
        return false;
    }

    return true;
}

void particle_engine::update(double t, float dt) {

}

void particle_engine::draw(const renderer::camera& cam) {
    if(s.active_buffer == 0) {   // Framebuffer 0 target was previously active (so we write to 1 & read from 0)
        s.buffers[1].bind();
        s.sim_pass.bind();
        s.sim_pass.bind_uniform("dir", vec2f(std::cos(s.angle), std::sin(s.angle)));
        s.buffers[0].get_attachment(0).bind(0);
        s.buffers[0].get_attachment(1).bind(1);
        s.buffers[0].get_attachment(2).bind(2);

        s.quad_mesh.bind();
        s.quad_mesh.draw();
        s.quad_mesh.release();

        s.buffers[0].get_attachment(0).release();
        s.buffers[0].get_attachment(1).release();
        s.buffers[0].get_attachment(2).release();

        s.sim_pass.release();

        s.buffers[1].release();

        s.buffers[0].bind();
        if(!s.buffers[0].read_attachment(s.copy_buffer, vec4i(0, 0, s.dim, s.dim), 0)) {    // Read from 0
            LOG("Error reading framebuffer (0, 0)");
            return;
        }
        s.buffers[0].release();

        // TODO: This step is unnecessary - bind pixel_buffer as vertex buffer directly, refactor
        s.particle_vbuf.bind();
        s.copy_buffer.bind(buffer_type::BT_PIXEL_UNPACK);
        s.particle_vbuf.copy_buffer(buffer_type::BT_PIXEL_UNPACK, buffer_type::BT_ARRAY, 0, 0, s.particle_count*sizeof(rgb32f_t));
        s.copy_buffer.release(buffer_type::BT_PIXEL_UNPACK);
        s.particle_vbuf.release();

        s.active_buffer = 1;
    } else if(s.active_buffer == 1) {   // Framebuffer 1 target was previously active (so we write to 0 & read from 1)
        s.buffers[0].bind();
        s.sim_pass.bind();
        s.sim_pass.bind_uniform("dir", vec2f(std::cos(s.angle), std::sin(s.angle)));
        s.buffers[1].get_attachment(0).bind(0);
        s.buffers[1].get_attachment(1).bind(1);
        s.buffers[1].get_attachment(2).bind(2);

        s.quad_mesh.bind();
        s.quad_mesh.draw();
        s.quad_mesh.release();

        s.buffers[1].get_attachment(0).release();
        s.buffers[1].get_attachment(1).release();
        s.buffers[1].get_attachment(2).release();

        s.sim_pass.release();

        s.buffers[0].release();

        s.buffers[1].bind();
        if(!s.buffers[1].read_attachment(s.copy_buffer, vec4i(0, 0, s.dim, s.dim), 0)) {    // Read from 1
            LOG("Error reading framebuffer (1, 0)");
            return;
        }
        s.buffers[1].release();

        s.particle_vbuf.bind();
        s.copy_buffer.bind(buffer_type::BT_PIXEL_UNPACK);
        s.particle_vbuf.copy_buffer(buffer_type::BT_PIXEL_UNPACK, buffer_type::BT_ARRAY, 0, 0, s.particle_count*sizeof(rgb32f_t));
        s.copy_buffer.release(buffer_type::BT_PIXEL_UNPACK);
        s.particle_vbuf.release();

        s.active_buffer = 0;
    }

    s.rndr_pass.bind();
    s.rndr_pass.bind_uniform("PVM", cam.proj_view());
    s.active_buffer == 0 ? s.buffers[1].get_attachment(2).bind(0) : s.buffers[0].get_attachment(2).bind(0);

    s.particle_mesh.bind();
    s.particle_mesh.draw();
    s.particle_mesh.release();

    s.active_buffer == 0 ? s.buffers[1].get_attachment(2).release() : s.buffers[0].get_attachment(2).release();
    s.rndr_pass.release();

    s.angle = maths::wrap(s.angle + 0.2f, float(-PI), float(PI));
}

const char* const particle_sim_vshdr = GLSL(
    in vec2 position;
    in vec2 texcoord1;

    out vec2 texcoord;

    void main() {
        texcoord = texcoord1;
        gl_Position = vec4(position.xy, 0.f, 1.f);
    }
);

const char* const particle_sim_fshdr = GLSL(
    uniform vec2 dir;

    uniform sampler2D position_tex;
    uniform sampler2D velocity_tex;
    uniform sampler2D colour_tex;

    in vec2 texcoord;

    out vec3 frag_colour[3];

    void main() {
        vec3 vel = texture(velocity_tex, texcoord).rgb;
        vec3 pos = texture(position_tex, texcoord).rgb + .02*vel;
        bool renew = length(pos) > 3.+(length(vel));
        frag_colour[0] = renew ? vec3(0., 0., 0.) : pos;
        frag_colour[1] = .99995*vel;
        frag_colour[2] = renew ? max(dot(normalize(vel.xy), dir), 0.3)*vec3(1,0,0) : texture(colour_tex, texcoord).rgb;
    }
);

const char* const particle_rndr_vshdr = GLSL(
    uniform mat4 PVM;

    in vec3 position;
    flat out int vertex_id;

    void main() {
        vertex_id = gl_VertexID;
        gl_Position = PVM * vec4(position, 1.f);
    }
);

const char* const particle_rndr_fshdr = GLSL(
    uniform int dim;
    uniform sampler2D colour_tex;

    flat in int vertex_id;

    out vec4 frag_colour;

    void main() {
        frag_colour = vec4(texelFetch(colour_tex, ivec2(vertex_id%dim, vertex_id/dim), 0).rgb, 1.);
    }
);
