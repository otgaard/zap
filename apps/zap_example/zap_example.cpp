/* Created by Darren Otgaar on 2016/03/05. http://www.github.com/otgaard/zap */
#define LOGGING_ENABLED
#include <maths/io.hpp>
#include <tools/log.hpp>

#include "shader_src.hpp"
#include "apps/application.hpp"

#include <engine/engine.hpp>
#include <engine/mesh.hpp>
#include <maths/algebra.hpp>
#include <engine/program.hpp>
#include <engine/framebuffer.hpp>
#include <generators/geometry/geometry2.hpp>
#include <generators/geometry/geometry3.hpp>
#include <engine/uniform_block.hpp>
#include <engine/uniform_buffer.hpp>
#include <renderer/colour.hpp>
#include <generators/noise/value_noise.hpp>
#include <maths/curves/curves.hpp>
#include <generators/textures/convolution.hpp>
#include <generators/noise/perlin.hpp>
#include "plotter2.hpp"
#include <generators/noise/noise.hpp>

using namespace zap;
using namespace zap::maths;
using namespace zap::engine;

using p2_t = core::position<vec2s>;
using vtx_p2_t = vertex<p2_t>;
using vbuf_p2_t = vertex_buffer<vtx_p2_t, buffer_usage::BU_STATIC_DRAW>;
using mesh_p2_t = mesh<vertex_stream<vbuf_p2_t>, primitive_type::PT_TRIANGLE_FAN>;

using pos3_t = core::position<vec3f>;
using nor3_t = core::normal<vec3f>;
using tex2_t = core::texcoord1<vec2f>;
using vtx_p3n3t2_t = vertex<pos3_t, nor3_t, tex2_t>;
using vbuf_p3n3t2_t = vertex_buffer<vtx_p3n3t2_t, buffer_usage::BU_STATIC_DRAW>;
using idx_us16_t = index_buffer<uint16_t, primitive_type::PT_TRIANGLE_STRIP, buffer_usage::BU_STATIC_DRAW>;

using mesh_p3n3t2_t = mesh<vertex_stream<vbuf_p3n3t2_t>, primitive_type::PT_TRIANGLES>;
using mesh_p3n3t2_tf_t = mesh<vertex_stream<vbuf_p3n3t2_t>, primitive_type::PT_TRIANGLE_STRIP, idx_us16_t>;

using transform_block = uniform_block<
        core::scale<float>,
        core::mv_matrix<mat4f>,
        core::cam_projection<mat4f>>;
using transform_uniform = uniform_buffer<transform_block, buffer_usage::BU_DYNAMIC_DRAW>;

class zap_example : public application {
public:
    zap_example() : application("zap_example", 1280, 768, false) { }

    void initialise() final;
    void update(double t, float dt) final;
    void draw() final;
    void shutdown() final;

    void on_resize(int width, int height) final;

protected:
    mesh_p2_t screenquad;
    vbuf_p2_t sq_buffer;
    program prog1;
    program prog2;
    program prog3;
    framebuffer framebuffer1;
    mesh_p3n3t2_t cube;
    vbuf_p3n3t2_t cube_buffer;
    transform_uniform uni1;
    texture test_tex;
    mesh_p3n3t2_tf_t cylinder;
    idx_us16_t cylinder_index;
    vbuf_p3n3t2_t cylinder_buffer;
    plotter2 plotter;
};

void zap_example::initialise() {
    prog1.add_shader(new shader(shader_type::ST_VERTEX, vtx_src));
    prog1.add_shader(new shader(shader_type::ST_FRAGMENT, frg_src));
    if(!prog1.link(true)) {
        LOG_ERR("Couldn't link prog1.");
        return;
    }

    prog2.add_shader(new shader(shader_type::ST_VERTEX, vtx_src));
    prog2.add_shader(new shader(shader_type::ST_FRAGMENT, tex_frg_src));
    if(!prog2.link(true)) {
        LOG_ERR("Couldn't link prog2.");
        return;
    }

    prog3.add_shader(new shader(shader_type::ST_VERTEX, cube_vtx_src));
    prog3.add_shader(new shader(shader_type::ST_FRAGMENT, cube_frg_src));
    if(!prog3.link(true)) {
        LOG_ERR("Couldn't link prog3.");
        return;
    }

    mat4f proj_matrix = {
        2.f, 0.f, 0.f, -1.f,
        0.f, 2.f, 0.f, -1.f,
        0.f, 0.f, 2.f, -1.f,
        0.f, 0.f, 0.f,  1.f
    };

    prog1.bind();
    prog1.bind_uniform("proj_matrix", proj_matrix);
    prog1.bind_uniform("colour", vec3f(1,1,0));
    gl_error_check();

    framebuffer1.allocate();
    framebuffer1.initialise<rgb888_t>(1, 1024, 1024, false, true);
    gl_error_check();

    prog2.bind();
    prog2.bind_uniform("proj_matrix", proj_matrix);
    prog2.bind_uniform("tex", 0);
    prog2.release();

    uni1.allocate();
    uni1.bind();
    gl_error_check();
    uni1.initialise(nullptr);
    if(uni1.map(buffer_access::BA_WRITE_ONLY)) {
        auto& ref = uni1.ref();
        ref.cam_projection = make_perspective<float>(45.f, 1280.f/768.f, 1.f, 100.f);
        ref.mv_matrix = make_translation<float>(0, 0, -3.f) * make_rotation(vec3f(1,0,0), PI/2);
        uni1.unmap();
    }

    prog3.bind();
    auto loc = prog3.uniform_block_index("transform");
    uni1.bind_point(loc);
    uni1.release();
    prog3.release();

    if(!screenquad.allocate() || !sq_buffer.allocate()) {
        LOG_ERR("Could not allocate screenquad mesh or buffer.");
        return;
    }

    screenquad.set_stream(&sq_buffer);

    screenquad.bind();
    sq_buffer.bind();

    std::vector<vtx_p2_t> vertices = { {{{0, 0}}}, {{{1, 0}}}, {{{1, 1}}}, {{{0, 1}}} };

    sq_buffer.initialise(vertices);
    screenquad.release();

    if(!cube.allocate() || !cube_buffer.allocate()) {
        LOG_ERR("Could not allocate cube mesh or buffer.");
        return;
    }

    cube.set_stream(&cube_buffer);

    cube.bind();
    cube_buffer.bind();
    auto v = generators::geometry3<vtx_p3n3t2_t, primitive_type::PT_TRIANGLES>::make_cube<float>(vec3f(1,1,1));
    cube_buffer.initialise(v);

    // Initialise testing texture
    test_tex.allocate();

    // Initialise noise PRN tables
    generators::noise::initialise(0);

    generators::value_noise<float> vnoise_gen;
    generators::perlin<float> pnoise_gen;

    using noise = generators::noise;

    const auto cols = 1024, rows = 512;
    const auto inv_c = float(TWO_PI)/(cols-1), inv_r = 16.f/rows;
    //auto inv_c = 1.f/cols, inv_r = 1.f/rows;
    UNUSED(inv_c); UNUSED(inv_r);

    std::vector<rgb888_t> pixels(cols*rows);

    for(size_t r = 0; r != rows; ++r) {
        for(size_t c = 0; c != cols; ++c) {
            const float theta = (c%(cols-1))*inv_c, ctheta = std::cos(theta), stheta = std::sin(theta);
            float value;

            //float value = 2*pnoise_gen.turbulence(8*ctheta, 8*stheta, r*inv_r, 4);

            if(r < 256) value = 2*noise::turbulence(pnoise_gen, 4, .5f, 2.f, 8*ctheta, 8*stheta, r*inv_r);
            else        value = noise::turbulence(vnoise_gen, 4, .5f, 2.f, 8*ctheta, 8*stheta, r*inv_r);

            /*
            if(r < 256) value = scale_bias(pnoise.fractal(8*ctheta, 8*stheta, r*inv_r, 6), 1.f, .5f);
            else        value = scale_bias(vnoise.fractal(8*ctheta, 8*stheta, r*inv_r, 6), .5f, .5f);
            */

            //LOG(value);
            //const float value = scale_bias(vnoise.fractal(2*ctheta, 8*stheta, r*inv_r, 4, .5f, 2.f), .5f, .5f);
            //const float value = vnoise.turbulence(8*ctheta, 8*stheta, r*inv_r, 4);
            //const float value = scale_bias(noise.fractal(8*ctheta, 8*stheta, r*inv_r, 6), .5f, .5f);
            vec3b P = lerp(value, colour::green8, colour::black8);
            //if(value < min) min = value; if(value > max) max = value;
            //auto P = bilinear(c*inv_c, r*inv_r, colour::red8, colour::green8, colour::yellow8, colour::blue8);
            //auto P = vec3b(rnd.rand()%256, rnd.rand()%256, rnd.rand()%256);
            pixels[cols*r+c].set3(P.x, P.y, P.z);
        }
    }

    //pixels = generators::convolution<rgb888_t>::boxblur(cols, rows, 32, pixels);

    test_tex.initialise(cols, rows, pixels, true);

    // Build the cylinder
    if(!cylinder.allocate() || !cylinder_buffer.allocate() || !cylinder_index.allocate()) {
        LOG_ERR("Could not allocate cylinder mesh or buffer.");
        return;
    }

    cylinder.set_stream(&cylinder_buffer);
    cylinder.set_index(&cylinder_index);
    cylinder.bind();
    cylinder_buffer.bind();
    auto cyl = generators::geometry3<vtx_p3n3t2_t, primitive_type::PT_TRIANGLE_STRIP>::make_cylinder(1.f, 2.f, 60, 4);
    cylinder_buffer.initialise(get<0>(cyl));
    cylinder_index.bind();
    cylinder_index.initialise(get<1>(cyl));
    cylinder.release();

    // Initialising plotter
    if(!plotter.initialise()) {
        LOG_ERR("Could not initialise plotter");
        return;
    }

    gl_error_check();
}

void zap_example::on_resize(int width, int height) {
    application::on_resize(width, height);

    mat4f proj_matrix = {
        2.f, 0.f, 0.f, -1.f,
        0.f, 2.f, 0.f, -1.f,
        0.f, 0.f, 2.f, -1.f,
        0.f, 0.f, 0.f,  1.f
    };

    prog1.bind();
    prog1.bind_uniform("proj_matrix", proj_matrix);
    prog1.release();
    gl_error_check();
}

static float rot = 0.0f;

void zap_example::update(double t, float dt) {
    uni1.bind();
    gl_error_check();

    //static auto fnc2 = curves::curve2<float, decltype(std::cosf), decltype(std::sinf)>::make_fnc(std::cosf, std::sinf);
    //auto P = fnc2(rot);

    if(uni1.map(buffer_access::BA_WRITE_ONLY)) {
        auto& ref = uni1.ref();
        ref.mv_matrix = make_translation<float>(0, 0.f, -3.f) *
                        make_rotation(vec3f(0,1,0), rot) *
                        make_rotation(vec3f(1,0,0), PI/2);
        uni1.unmap();
    }

    rot = wrap<float>(rot + dt, -TWO_PI, TWO_PI);
}

void zap_example::draw() {
    depth_test(true);
    prog3.bind();
    //cube.bind();
    //cube_buffer.bind();

    cylinder.bind();

    test_tex.bind(0);
    cylinder.draw();
    test_tex.release();

    //cube_buffer.release();
    //cube.release();
    cylinder.release();
    prog3.release();
    depth_test(false);

    plotter.draw();

    /*
    screenquad.bind();
    prog2.bind();
    test_tex.bind(0);
    //active->get_attachment(0).bind();
    screenquad.draw();
    test_tex.release();
    //active->get_attachment(0).release();
    prog2.release();

    screenquad.release();
    */
}

void zap_example::shutdown() {
}

int main(int argc, char* argv[]) {
    zap_example app;
    return app.run();
}
