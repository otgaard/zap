/* Created by Darren Otgaar on 2016/06/11. http://www.github.com/otgaard/zap */
#include "metaballs.hpp"

/* Created by Darren Otgaar on 2016/06/10. http://www.github.com/otgaard/zap */
#include "bars.hpp"
#include "../graphic_types.hpp"
#include "../application.hpp"

#include <generators/textures/planar.hpp>
#include <generators/geometry/surface.hpp>

using namespace zap;
using namespace zap::maths;
using namespace zap::engine;

const char* metaballs_vshdr = GLSL(
        in vec3 position;
        in vec3 normal;

        layout (std140) uniform transform {
            mat4 mv_matrix;
            mat4 proj_matrix;
        };

        out vec3 nor;

        void main() {
            nor = normal;
            gl_Position = proj_matrix * mv_matrix * vec4(position.x, position.y, position.z, 1.0);
        }
);

const char* metaballs_fshdr = GLSL(
        in vec3 nor;

        out vec4 frag_colour;
        void main() {
            float s = max(dot(nor, normalize(vec3(0,10,10))), 0);
            frag_colour = s*vec4(nor, 1);
        }
);

/*
 * Uniform Block, Vertex Type, and Mesh Defintions
 */

using metaballs_block = uniform_block<
        core::mv_matrix<mat4f>,
        core::cam_projection<mat4f>>;
using metaballs_uniform = uniform_buffer<metaballs_block, buffer_usage::BU_DYNAMIC_DRAW>;

using metaballs_vbuf_t = vertex_buffer<vtx_p3n3_t, buffer_usage::BU_DYNAMIC_COPY>;
using metaballs_mesh_t = mesh<vertex_stream<metaballs_vbuf_t>, primitive_type::PT_TRIANGLES>;

struct metaballs::state_t {
    program metaballs_program;
    metaballs_vbuf_t metaballs_buffer;
    metaballs_mesh_t metaballs_mesh;
    metaballs_uniform uniform;
    std::vector<vtx_p3n3_t> blob;
};

metaballs::metaballs(application* app_ptr) : module(app_ptr, "metaballs") {
    state = std::make_unique<state_t>();

    // The OpenGL Context should be valid here
    auto& s = *state.get();

    s.metaballs_program.add_shader(new shader(shader_type::ST_VERTEX, metaballs_vshdr));
    s.metaballs_program.add_shader(new shader(shader_type::ST_FRAGMENT, metaballs_fshdr));
    if(!s.metaballs_program.link(true)) gl_error_check();

    s.uniform.allocate();
    s.uniform.bind();
    gl_error_check();
    s.uniform.initialise(nullptr);
    if(s.uniform.map(buffer_access::BA_WRITE_ONLY)) {
        auto& ref = s.uniform.ref();
        ref.cam_projection = make_perspective2<float>(45.f, 1280.f / 768.f, 1.f, 100.f);
        ref.mv_matrix = make_scale<float>(30,30,10) * make_translation<float>(-.5f, -.5f, -2.f);
        s.uniform.unmap();
    }

    const size_t max_tris = 40000;

    s.metaballs_mesh.allocate();
    s.metaballs_mesh.bind();
    s.metaballs_mesh.set_stream(&s.metaballs_buffer);
    s.metaballs_buffer.allocate();
    s.metaballs_buffer.bind();
    s.metaballs_buffer.initialise(max_tris);

    auto fnc = [](float x, float y, float z) {
        auto a = vec3f(x,y,z) - vec3f(0.5f) ;
        return 0.2f/dot(a,a);
    };

    s.blob.reserve(max_tris);
    generators::surface<metaballs_vbuf_t>::marching_cubes(fnc, s.blob);
    LOG("Finished building surface", s.blob.size());
    s.metaballs_buffer.copy(s.blob, 0, s.blob.size());
    s.metaballs_mesh.release();

    s.metaballs_program.bind();
    auto loc = s.metaballs_program.uniform_block_index("transform");
    s.uniform.bind_point(loc);
    s.uniform.release();
    s.metaballs_program.release();
}

metaballs::~metaballs() {
}

void metaballs::update(double t, float dt) {
    // Use the analysis to create a metaballs function
    auto& s = *state;
    s.blob.clear();

    const auto* analysis_ptr = analysis_.data();

    auto fnc = [analysis_ptr](float x, float y, float z) {
        const auto P = vec3f(x,y,z);
        auto a = P - vec3f(0.5f+analysis_ptr[0]*0.3f, 0.5f+analysis_ptr[1]*0.3f, 0.5f+analysis_ptr[10]*0.3f);
        auto b = P - vec3f(0.5f-analysis_ptr[2]*0.3f, 0.5f+analysis_ptr[3]*0.3f, 0.5f+analysis_ptr[11]*0.3f);
        auto c = P - vec3f(0.5f+analysis_ptr[4]*0.3f, 0.5f-analysis_ptr[5]*0.3f, 0.5f+analysis_ptr[12]*0.3f);
        auto d = P - vec3f(0.5f-analysis_ptr[6]*0.3f, 0.5f-analysis_ptr[7]*0.3f, 0.5f+analysis_ptr[13]*0.3f);
        return analysis_ptr[6]/dot(a,a) + analysis_ptr[7]/dot(b,b) + analysis_ptr[8]/dot(c,c) + analysis_ptr[9]/dot(d,d);
    };

    generators::surface<metaballs_vbuf_t>::marching_cubes(fnc, s.blob);
    s.metaballs_buffer.bind();
    s.metaballs_buffer.copy(s.blob, 0, s.blob.size());
    s.metaballs_buffer.release();
    gl_error_check();
}

void metaballs::draw() {
    auto& s = *state.get();

    app_ptr_->depth_test(true);

    s.metaballs_program.bind();
    s.metaballs_mesh.bind();
    s.metaballs_mesh.draw(primitive_type::PT_TRIANGLES, 0, s.blob.size());
    s.metaballs_mesh.release();
    s.metaballs_program.release();

    app_ptr_->depth_test(false);
}

