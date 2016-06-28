/* Created by Darren Otgaar on 2016/06/11. http://www.github.com/otgaard/zap */
#include "metaballs.hpp"
#include "apps/graphic_types.hpp"
#include "apps/application.hpp"

#include <generators/textures/planar.hpp>
#include <generators/geometry/surface.hpp>

using namespace zap;
using namespace zap::maths;
using namespace zap::engine;

const char* metaballs_vshdr = GLSL(
        in vec3 position;
        in vec3 normal;

        layout (std140) uniform transform {
            float scale;
            mat4 mv_matrix;
            mat4 proj_matrix;
        };

        out vec3 nor;
        out vec3 tex;
        out float freq;

        void main() {
            freq = scale;
            nor = normal;
            tex = position.xyz;
            gl_Position = proj_matrix * mv_matrix * vec4(position.x, position.y, position.z, 1.0);
        }
);

const char* metaballs_fshdr = GLSL(
        uniform sampler2D diffuse;
        in vec3 nor;
        in vec3 tex;
        in float freq;

        out vec4 frag_colour;
        void main() {
            float s = max(dot(nor, vec3(0,0,1)), 0);
            vec3 colour = mix(vec3(1,0,0), vec3(0,0,1), texture(diffuse, tex.xy).s);
            frag_colour = vec4(s*colour, 0.8);
        }
);

/*
 * Uniform Block, Vertex Type, and Mesh Defintions
 */

using metaballs_block = uniform_block<
        core::scale<float>,
        core::mv_matrix<mat4f>,
        core::cam_projection<mat4f>>;
using metaballs_uniform = uniform_buffer<metaballs_block, buffer_usage::BU_DYNAMIC_DRAW>;

using metaballs_vbuf_t = vertex_buffer<vtx_p3n3t2_t, buffer_usage::BU_DYNAMIC_COPY>;
using metaballs_mesh_t = mesh<vertex_stream<metaballs_vbuf_t>, primitive_type::PT_TRIANGLES>;

struct metaballs::state_t {
    program metaballs_program;
    metaballs_vbuf_t metaballs_buffer;
    metaballs_mesh_t metaballs_mesh;
    metaballs_uniform uniform;
    std::vector<vtx_p3n3t2_t> blob;
    texture tex1;

    float rotation; // For testing
};

metaballs::metaballs(application* app_ptr) : module(app_ptr, "metaballs") {
    state = std::make_unique<state_t>();

    // The OpenGL Context should be valid here
    auto& s = *state.get();

    s.rotation = 0.f;

    s.metaballs_program.add_shader(new shader(shader_type::ST_VERTEX, metaballs_vshdr));
    s.metaballs_program.add_shader(new shader(shader_type::ST_FRAGMENT, metaballs_fshdr));
    if(!s.metaballs_program.link(true)) gl_error_check();

    s.tex1.allocate();
    s.tex1.bind();
    s.tex1.initialise(32, 32, generators::planar<rgb332_t>::make_checker(32, 32, colour::black8, colour::white8), true);

    s.metaballs_program.bind();
    auto tex_loc = s.metaballs_program.uniform_location("diffuse");
    s.metaballs_program.bind_texture_unit(tex_loc, 0);
    s.metaballs_program.release();
    s.tex1.release();

    s.uniform.allocate();
    s.uniform.bind();
    gl_error_check();
    s.uniform.initialise(nullptr);
    if(s.uniform.map(buffer_access::BA_WRITE_ONLY)) {
        auto& ref = s.uniform.ref();
        ref.cam_projection = make_perspective<float>(45.f, 1280.f / 768.f, 1.f, 100.f);
        ref.mv_matrix = make_scale<float>(30,30,10) * make_translation<float>(-.5f, -.5f, -2.f);
        s.uniform.unmap();
    }

    const size_t max_tris = 50000;

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

    constexpr static float dtheta = float(TWO_PI/16);
    static vec3f positions[16];
    static float mag[16];
    for(int i = 0; i < 16; ++i) {
        float theta = i*dtheta;
        mag[i] = .4f*0.5f*(analysis_[2*i] + analysis_[2*i+1]);
        positions[i] = vec3f(.5f + mag[i]*std::cos(theta), .5f + mag[i]*std::sin(theta), .5f + ((i%2) ? -0.05f : +0.05f));
    }

    auto fnc = [=](float x, float y, float z) {
        const auto P = vec3f(x,y,z);
        float result = 0;
        for(int i = 0; i != 16; ++i) {
            auto d = P - positions[i];
            result += mag[i] / dot(d, d);
        }
        return result;
    };

    generators::surface<metaballs_vbuf_t>::marching_cubes(fnc, s.blob);
    s.metaballs_buffer.bind();
    s.metaballs_buffer.copy(s.blob, 0, s.blob.size());
    s.metaballs_buffer.release();
    gl_error_check();

    static loop<float> y_angle = loop<float>(-PI/6, PI/6);
    static loop<float> x_angle = loop<float>(-PI/6, PI/6);
    y_angle += 0.5f*dt; x_angle += 0.3f*dt;

    s.uniform.bind();
    if(s.uniform.map(buffer_access::BA_READ_WRITE)) {
        auto& ref = s.uniform.ref();
        ref.mv_matrix = make_scale<float>(3,3,3) *
                make_translation<float>(0,0,-0.65f) *
                make_rotation(vec3f(1,0,0), x_angle.value) *
                make_rotation(vec3f(0,1,0), y_angle.value) *
                make_rotation(vec3f(0,0,1), s.rotation) *
                make_translation<float>(-.5f, -.5f, -.5f);
        s.uniform.unmap();
    }
    s.uniform.release();

    static loop<float> dir(-0.07f, 0.07f);
    dir += 0.01f*mag[0]*dt;
    s.rotation = wrap<float>(s.rotation + dir.value, -TWO_PI, TWO_PI);

}

void metaballs::draw() {
    auto& s = *state.get();

    app_ptr_->depth_test(true);
    //app_ptr_->alpha_blending(true);

    s.metaballs_program.bind();
    s.tex1.bind();
    s.metaballs_mesh.bind();
    s.metaballs_mesh.draw(primitive_type::PT_TRIANGLES, 0, s.blob.size());
    s.metaballs_mesh.release();
    s.tex1.release();
    s.metaballs_program.release();

    //app_ptr_->alpha_blending(false);
    app_ptr_->depth_test(false);
}

