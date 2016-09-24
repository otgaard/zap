/* Created by Darren Otgaar on 2016/06/11. http://www.github.com/otgaard/zap */
#include "metaballs.hpp"
#include "apps/graphic_types.hpp"
#include "apps/application.hpp"
#include <generators/geometry/surface.hpp>
#include <generators/noise/noise.hpp>

#include <maths/io.hpp>

using namespace zap;
using namespace zap::maths;
using namespace zap::engine;

const char* metaballs_vshdr = GLSL(
        in vec3 position;
        in vec3 normal;

        layout (std140) uniform transform {
            mat4 mv_matrix;
            mat4 proj_matrix;
            float scale;
        };

        out vec3 nor;
        out vec3 tex;

        void main() {
            nor = vec3(mv_matrix * vec4(normal,0));
            tex = vec3(position.xy, position.z + scale);
            gl_Position = proj_matrix * mv_matrix * vec4(position.x, position.y, position.z, 1.0);
        }
);

const char* metaballs_fshdr = GLSL(
        uniform usampler1D diffuse;
        in vec3 nor;
        in vec3 tex;

        int prn(int x) { return int(texelFetch(diffuse, x & 0xFF, 0).r); }
        int prn2(int x, int y) { return prn(x + prn(y)); }
        int prn3(int x, int y, int z) { return prn(x + prn(y + prn(z))); }

        float bilinear1(float u, float v, float P00, float P01, float P10, float P11) {
            float omu = 1.0 - u; return (1.0 - v)*(P00*omu + P01*u) + v*(P10*omu + P11*u);
        }

        float vnoise2(float x, float y) {
            int xi = int(x); int yi = int(y);
            float dx = x - xi; float dy = y - yi;
            return bilinear1(dx, dy, prn2(xi, yi), prn2(xi+1, yi), prn2(xi, yi+1), prn2(xi+1, yi+1))/255.0;
        }

        float vnoise3(float x, float y, float z) {
            int xi = int(floor(x)); int yi = int(floor(y)); int zi = int(floor(z));
            float dx = x - xi; float dy = y - yi; float dz = z - zi;
            return mix(bilinear1(dx, dy, prn3(xi, yi, zi),   prn3(xi+1, yi, zi),   prn3(xi, yi+1, zi),   prn3(xi+1, yi+1, zi)),
                       bilinear1(dx, dy, prn3(xi, yi, zi+1), prn3(xi+1, yi, zi+1), prn3(xi, yi+1, zi+1), prn3(xi+1, yi+1, zi+1)),
                       dz)/255.0;
        }

        float turbulence2(float x, float y, int octaves, float persistence, float lacunarity) {
            float freq = 1.0; float ampl = 1.0; float accum = 0.0; float mag = 0.0;
            for(int i = 0; i != octaves; ++i) {
                accum += abs(ampl * vnoise2(freq * x, freq * y));
                mag += ampl;
                ampl *= persistence;
                freq *= lacunarity;
            }
            return 1.0/mag*accum;
        }

        float turbulence3(float x, float y, float z, int octaves, float persistence, float lacunarity) {
            float freq = 1.0; float ampl = 1.0; float accum = 0.0; float mag = 0.0;
            for(int i = 0; i != octaves; ++i) {
                accum += abs(ampl * (2.0*vnoise3(freq * x, freq * y, freq * z) - 1.0));
                mag += ampl;
                ampl *= persistence;
                freq *= lacunarity;
            }
            return 1.0/mag*accum;
        }

        const vec3 light_dir = vec3(0,0,1);

        out vec4 frag_colour;
        void main() {
            float s = max(dot(nor, light_dir), 0);
            vec3 colour = mix(vec3(0,0.75,0), vec3(0,0,0), turbulence3(30*tex.x, 30*tex.y, 30*tex.z, 4, .5, 2.));
            frag_colour = vec4(s*colour, 1.);
        }
);

/*
 * Uniform Block, Vertex Type, and Mesh Defintions
 */

using metaballs_block = uniform_block<
        core::mv_matrix<mat4f>,
        core::cam_projection<mat4f>,
        core::scale<float>>;
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

    if(!generators::noise::is_initialised()) generators::noise::initialise();

    if(!s.tex1.allocate()) { LOG_ERR("Couldn't initialise noise PRN table texture"); return; }
    s.tex1.initialise(texture_type::TT_TEX1D, 256, 1, pixel_format::PF_RED, pixel_datatype::PD_DN_UNSIGNED_BYTE, generators::noise::prn_tbl_ptr());

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
        ref.cam_projection = make_perspective<float>(45.f, 1280.f / 768.f, .5f, 100.f);
        ref.mv_matrix = make_scale<float>(30,30,10) * make_translation<float>(-.5f, -.5f, -2.f);
        //ref.normal_matrix = ref.mv_matrix.rotation().inverse().transpose();
        ref.scale = 1.f;
        s.uniform.unmap();
    }

    const size_t max_tris = 50000;
    gl_error_check();

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
        mag[i] = .4f*0.5f*(window_[2*i] + window_[2*i+1]);
        positions[i] = vec3f(.5f + mag[i]*std::cos(theta), .5f + mag[i]*std::sin(theta), .5f + ((i%2) ? -0.5f*mag[i] : +0.5f*mag[i]));
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
    static loop<float> x_angle = loop<float>(-PI, PI);
    y_angle += 0.5f*dt; x_angle += 0.3f*dt;

    s.uniform.bind();
    if(s.uniform.map(buffer_access::BA_READ_WRITE)) {
        auto& ref = s.uniform.ref();
        ref.mv_matrix =
                make_translation<float>(0,0,-1.2f) *
                make_rotation(vec3f(1,0,0), x_angle.value) *
                make_rotation(vec3f(0,1,0), y_angle.value) *
                make_rotation(vec3f(0,0,1), s.rotation) *
                make_translation<float>(-.5f, -.5f, -.5f);
        ref.scale += 0.001f*analysis_.estimated_beat;
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

    s.metaballs_program.bind();
    s.tex1.bind();
    s.metaballs_mesh.bind();
    s.metaballs_mesh.draw(primitive_type::PT_TRIANGLES, 0, s.blob.size());
    s.metaballs_mesh.release();
    s.tex1.release();
    s.metaballs_program.release();

    app_ptr_->depth_test(false);
}
