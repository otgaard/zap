/* Created by Darren Otgaar on 2016/10/28. http://www.github.com/otgaard/zap */

#include <apps/graphic_types.hpp>
#include <maths/rand_lcg.hpp>
#include "part_engine.hpp"

using namespace zap;
using namespace zap::maths;
using namespace zap::engine;
using namespace zap::effects;

// We need a "quad" of dims [particle_count,1] for rendering

using quad_vertex_t = vertex<pos2f_t, tex2f_t>;
using quad_vbuf_t = vertex_buffer<quad_vertex_t, buffer_usage::BU_STATIC_DRAW>;
using quad_mesh_t = mesh<vertex_stream<quad_vbuf_t>, primitive_type::PT_TRIANGLE_FAN>;

using vec3_pbuf_t = pixel_buffer<rgb32f_t, buffer_usage::BU_DYNAMIC_DRAW>;

using particle_vbuf_t = vertex_buffer<vertex<pos3f_t>, buffer_usage::BU_STREAM_COPY>;
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

    // Temporary texture
    texture init_tex;
};

particle_engine::particle_engine() : state_(new state_t()), s(*state_.get()) {
}

particle_engine::~particle_engine() {
}

bool particle_engine::initialise() {
    s.allocated = 0;

    s.dim = (size_t)std::pow(2, std::ceil(std::log2(std::sqrt(s.particle_count))));
    LOG("PARTICLE ENGINE DIMENSIONS:", s.dim, s.dim*s.dim, s.particle_count);

    for(auto& buf : s.buffers) {
        if(!buf.allocate() || !buf.initialise(3, s.dim, s.dim, pixel_format::PF_RGB, pixel_datatype::PD_FLOAT, false, false)) {
            LOG_ERR("Failure allocating/initialising framebuffers");
            return false;
        }
    }

    std::vector<rgb32f_t> initial_conditions;
    initial_conditions.reserve(s.dim*s.dim);
    rand_lcg rand;

    for(int i = 0, end = s.dim*s.dim; i != end; ++i) {
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
    s.particle_vbuf.copy_buffer(buffer_type::BT_PIXEL_UNPACK, buffer_type::BT_ARRAY, 0, 0, s.particle_count);
    s.particle_mesh.release();

    s.copy_buffer.release();

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

    if(!s.init_tex.allocate() || !s.init_tex.initialise(s.copy_buffer)) {
        LOG_ERR("Initialisation Texture failed to initialise");
        return false;
    }

    return true;
}

void particle_engine::update(double t, float dt) {

}

void particle_engine::draw(const renderer::camera& cam) {
    s.buffers[0].bind();
    s.sim_pass.bind();
    s.sim_pass.bind_texture_unit(s.sim_pass.uniform_location("particle_tex"), 0);
    s.init_tex.bind(0);
    s.quad_mesh.bind();
    s.quad_mesh.draw();
    s.quad_mesh.release();
    s.init_tex.release();
    s.sim_pass.release();
    s.buffers[0].release();

    s.rndr_pass.bind();
    s.rndr_pass.bind_uniform("PVM", cam.proj_view());
    s.particle_mesh.bind();
    s.particle_mesh.draw();
    s.particle_mesh.release();
    s.rndr_pass.release();
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
    uniform sampler2D particle_tex;

    in vec2 texcoord;

    out vec3 frag_colour;

    void main() {
        frag_colour = texture(particle_tex, texcoord).rgb;
    }
);

const char* const particle_rndr_vshdr = GLSL(
    uniform mat4 PVM;

    in vec3 position;

    void main() {
        gl_Position = PVM * vec4(position, 1.f);
    }
);

const char* const particle_rndr_fshdr = GLSL(
    out vec4 frag_colour;

    void main() {
        frag_colour = vec4(1,1,1,1);
    }
);
