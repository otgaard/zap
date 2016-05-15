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

/*
 * Goals for this weekend:
 * 1) Finish up textures/pixels
 * 2) Add the mesh class
 * 3) Add framebuffers
 * 4) Add uniform buffers/blocks
 */

#include "shader_src.hpp"

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
            120/1280.f,       0.f, 0.f, 0.f,
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
    std::vector<vertex_t> box =
    {
            {
                    {{-10,-10,0}},  // Position
                    {{0,0,1}},      // Normal
                    {{0,0}}         // Texcoord
            },
            {
                    {{10,-10,0}},
                    {{0,0,1}},
                    {{1,0}}
            },
            {
                    {{10,10,0}},
                    {{0,0,1}},
                    {{1,1}}
            },
            {
                    {{-10,10,0}},
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
    my_block.cam_projection = proj_matrix;
    my_block.mv_matrix = proj_matrix;

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

    auto& cv = ublock1_ptr->get()->cam_view;
    cv = vec3f(osc_.get_value(), 1.f-osc_.get_value(), 0.0f);

    auto& pj = ublock1_ptr->get()->cam_projection;
    pj(1,1) = 100*osc_.get_value()/768.f;
    pj(0,0) = 100*osc_.get_value()/1280.f;

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
