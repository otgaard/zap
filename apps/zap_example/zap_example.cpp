/* Created by Darren Otgaar on 2016/03/05. http://www.github.com/otgaard/zap */
#define LOGGING_ENABLED
#include <tools/log.hpp>
#include <maths/mat4.hpp>
#include <maths/vec3.hpp>
#include <maths/vec2.hpp>
#include <engine/types.hpp>
#include <engine/shader.hpp>
#include <engine/program.hpp>
#include <maths/functions.hpp>
#include <engine/vertex_buffer.hpp>
#include <engine/pixel_buffer.hpp>
#include <engine/index_buffer.hpp>
#include <engine/mesh.hpp>
#include "generator.hpp"
#include "application.hpp"
#include <engine/uniform_buffer.hpp>

#include <GL/glew.h>
#include <generators/textures/spectral.hpp>
#include <geometry/ray.hpp>
/*
 * Goals for this weekend:
 * 1) Finish up textures/pixels
 * 2) Add the mesh class
 * 3) Add framebuffers
 * 4) Add uniform buffers/blocks
 */

#include "shader_src.hpp"
#include "maths/io.hpp"

using namespace zap;
using namespace zap::maths;
using namespace zap::engine;

using pos3_t = core::position<vec3f>;
using nor3_t = core::normal<vec3f>;
using tex2_t = core::texcoord1<vec2f>;

using vertex_t = vertex<pos3_t, nor3_t, tex2_t>;
using vertex_buffer_t = vertex_buffer<vertex_t, buffer_usage::BU_STATIC_DRAW>;
using vtx_stream_t = vertex_stream<vertex_buffer_t>;
using mesh_t = mesh<vtx_stream_t, primitive_type::PT_TRIANGLE_FAN>;

using ublock1 = uniform_block<
        core::cam_position<vec3f>,
        core::cam_view<vec3f>,
        core::mv_matrix<mat4f>,
        core::cam_projection<mat4f>>;
using ublock1_buffer = uniform_buffer<ublock1, buffer_usage::BU_DYNAMIC_DRAW>;

class zap_example : public application {
public:
    zap_example() : application("zap_example"), osc_(3.f) { }

    void initialise() final;
    void update(double t, float dt) final;
    void draw() final;
    void shutdown() final;

protected:
    std::unique_ptr<program> prog1;
    std::unique_ptr<program> prog2;
    std::unique_ptr<vertex_buffer_t> vbuffer_ptr;
    std::unique_ptr<mesh_t> mesh_ptr;
    std::unique_ptr<ublock1_buffer> ublock1_ptr;
    oscillator<float> osc_;
    std::unique_ptr<texture> tex_ptr;

    // Temp stuff
    mat4<float> translation;
    float angle;
    float distance;
};

void zap_example::initialise() {
    std::vector<shader_ptr> arr;
    arr.push_back(std::make_shared<shader>(shader_type::ST_VERTEX, vtx_src));
    arr.push_back(std::make_shared<shader>(shader_type::ST_FRAGMENT, frg_src));
    prog1 = std::make_unique<program>(std::move(arr));
    prog1->link(true);
    gl_error_check();

    std::vector<shader_ptr> arr2;
    arr2.push_back(std::make_shared<shader>(shader_type::ST_VERTEX, vtx_ub_src));
    arr2.push_back(std::make_shared<shader>(shader_type::ST_FRAGMENT, frg_ub_src));
    prog2 = std::make_unique<program>(std::move(arr2));
    prog2->link(true);
    gl_error_check();

    mat4f proj_matrix = {
            60/1280.f,       0.f, 0.f, 0.f,
            0.f,         60/768.f, 0.f, 0.f,
            0.f,              0.f, 2.f, 0.f,
            0.f,              0.f, 0.f, 1.f
    };

    prog1->bind();
    auto loc = prog1->uniform_location("proj_matrix");
    glUniformMatrix4fv(loc, 1, GL_FALSE, proj_matrix.data());
    loc = prog1->uniform_location("colour");
    auto line_colour = zap::maths::vec3f(1,0,0);
    glUniform3fv(loc, 1, line_colour.data());
    prog1->release();
    gl_error_check();


    prog2->bind();

    tex_ptr = std::make_unique<texture>();
    tex_ptr->allocate();

    auto pixels = zap::generators::spectral<rgb888_t>::make_clouds(5, 5, 1024, 1024);
    // Remap colours

    constexpr auto max_byte = std::numeric_limits<typename rgb888_t::type>::max();
    constexpr float inv_byte = 1.f/std::numeric_limits<typename rgb888_t::type>::max();

    const vec3f blue = vec3f(0./max_byte, 191./max_byte, 255./max_byte);
    const vec3f white = vec3f(180./max_byte, 206./max_byte, 235./max_byte);

    for(auto& pixel : pixels) {
        auto c = lerp(white, blue, pixel.get(0)*inv_byte);
        pixel.set(0, typename rgb888_t::type(c.x*max_byte));
        pixel.set(1, typename rgb888_t::type(c.y*max_byte));
        pixel.set(2, typename rgb888_t::type(c.z*max_byte));
    }

    tex_ptr->initialise(1024, 1024, pixels, false);
    //return ptr;

    //tex_ptr = generator::create_checker();
    tex_ptr->bind();
    loc = prog2->uniform_location("tex");
    glUniform1i(loc, 0);
    //tex_ptr->release();

    std::vector<vertex_t> box =
    {
            {
                    {{-1,-1,0}},  // Position
                    {{0,0,1}},      // Normal
                    {{0,0}}         // Texcoord
            },
            {
                    {{1,-1,0}},
                    {{0,0,1}},
                    {{1,0}}
            },
            {
                    {{1,1,0}},
                    {{0,0,1}},
                    {{1,1}}
            },
            {
                    {{-1,1,0}},
                    {{0,0,1}},
                    {{0,1}}
            }
    };

    vbuffer_ptr = std::make_unique<vertex_buffer_t>();
    mesh_ptr = std::make_unique<mesh_t>(vtx_stream_t(vbuffer_ptr.get()));
    mesh_ptr->allocate();
    mesh_ptr->bind();
    vbuffer_ptr->allocate();
    vbuffer_ptr->bind();
    vbuffer_ptr->initialise(box);

    // Now, let's create a uniform buffer & block
    ublock1 my_block;
    my_block.cam_position = vec3f(0,0,-10);
    my_block.cam_view = vec3f(1,1,0);
    my_block.cam_projection = make_perspective<float>(90.f, 1280.f/768.f, 1.f, 100.f);

    translation = make_translation(0.f,0.f,2.f);
    angle = 0;
    distance = 0;

    my_block.mv_matrix = translation;

    ublock1_ptr = std::make_unique<ublock1_buffer>();
    ublock1_ptr->allocate();
    ublock1_ptr->bind();
    ublock1_ptr->initialise(my_block);

    auto ublock1_loc = prog2->uniform_block_index("ublock1");
    ublock1_ptr->bind_point(ublock1_loc);
    ublock1_ptr->release();
    osc_.start();
}

void zap_example::update(double t, float dt) {
    ublock1_ptr->bind();
    ublock1_ptr->map(buffer_access::BA_READ_WRITE);

    angle += dt;
    distance += dt;
    ublock1_ptr->get()->mv_matrix = translation * make_rotation(vec3f(0,0,1), angle) * make_rotation(vec3f(1,0,0), angle);

    ublock1_ptr->unmap();
    ublock1_ptr->release();
}

void zap_example::draw() {
    mesh_ptr->draw();
}

void zap_example::shutdown() {

}

int main(int argc, char* argv[]) {
    zap_example app;
    return app.run();
}
