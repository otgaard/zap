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
#include <geometry/disc.hpp>
#include <generators/geometry/geometry2.hpp>
#include <generators/textures/planar.hpp>
#include <generators/geometry/surface.hpp>
#include <thread>

#include "shader_src.hpp"
#include "maths/io.hpp"

using namespace zap;
using namespace zap::maths;
using namespace zap::engine;

using pos3_t = core::position<vec3f>;
using nor3_t = core::normal<vec3f>;
using tex2_t = core::texcoord1<vec2f>;
using col3_t = core::colour1<vec3f>;

using vertex_t = vertex<pos3_t, nor3_t, tex2_t, col3_t>;
using vertex_buffer_t = vertex_buffer<vertex_t, buffer_usage::BU_DYNAMIC_DRAW>;
using vtx_stream_t = vertex_stream<vertex_buffer_t>;
using mesh_t = mesh<vtx_stream_t, primitive_type::PT_TRIANGLES>;

using ublock1 = uniform_block<
        core::cam_position<vec3f>,
        core::cam_view<vec3f>,
        core::mv_matrix<mat4f>,
        core::cam_projection<mat4f>>;
using transform_buffer = uniform_buffer<ublock1, buffer_usage::BU_DYNAMIC_DRAW>;

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

    vertex_buffer_t vbuffer;
    mesh_t mesh;
    transform_buffer transform;
    texture tex;
    std::vector<vertex_t> surf;
    oscillator<float> osc_;
};

void zap_example::initialise() {
    LOG("Number of Concurrent threads supported:", std::thread::hardware_concurrency());

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
            60/1280.f,      0.f, 0.f, 0.f,
            0.f,       60/768.f, 0.f, 0.f,
            0.f,            0.f, 2.f, 0.f,
            0.f,            0.f, 0.f, 1.f
    };

    mat4f mv_matrix = {
            6.4f, 0.f,  0.f, 0.f,
            0.f, 6.4f,  0.f, 0.f,
            0.f,  0.f, 6.4f, 0.f,
            0.f,  0.f,  0.f, 1.f
    };

    prog1->bind();
    auto loc = prog1->uniform_location("proj_matrix");
    glUniformMatrix4fv(loc, 1, GL_FALSE, proj_matrix.data());
    loc = prog1->uniform_location("colour");
    auto line_colour = zap::maths::vec3f(1,1,0);
    glUniform3fv(loc, 1, line_colour.data());
    loc = prog1->uniform_location("mv_matrix");
    glUniformMatrix4fv(loc, 1, GL_FALSE, mv_matrix.data());
    prog1->release();
    gl_error_check();


    prog2->bind();
    loc = prog2->uniform_location("tex");
    glUniform1i(loc, 0);

    tex.allocate();
    auto checker = zap::generators::planar<rgb888_t>::make_checker<vec3b>(32, 32, vec3b(255, 255, 0), vec3b(255, 0, 255));
    tex.initialise(32, 32, checker, true);
    tex.bind();

    std::vector<vertex_t> box =
    {
            {
                    {{-1,-1,0}},  // Position
                    {{0,0,1}},    // Normal
                    {{0,0}},      // Texcoord
                    {{0,0,0}}     // Colour1
            },
            {
                    {{1,-1,0}},
                    {{0,0,1}},
                    {{1,0}},
                    {{0,0,0}}
            },
            {
                    {{1,1,0}},
                    {{0,0,1}},
                    {{1,1}},
                    {{0,0,0}}
            },
            {
                    {{-1,1,0}},
                    {{0,0,1}},
                    {{0,1}},
                    {{0,0,0}}
            }
    };

    mesh.set_stream(vertex_stream<vertex_buffer_t>(&vbuffer));
    mesh.allocate();
    mesh.bind();
    vbuffer.allocate();
    vbuffer.bind();
    vbuffer.initialise(15000);
    //vbuffer.initialise(box);

    auto fnc = [](float x, float y, float z) {
        vec3f v(x-0.5f, y-0.5f, z-0.5f);
        vec3f u(x-0.7f, y-0.5f, z-0.5f);
        return 0.5f/dot(v,v) + 0.2f/dot(u,u);
    };
    surf.reserve(15000);
    generators::surface<vertex_buffer_t>::marching_cubes(fnc, surf);
    LOG("Finished building surface", surf.size());
    vbuffer.initialise(surf);

    transform.allocate();
    transform.bind();
    transform.initialise(nullptr);
    if(transform.map(buffer_access::BA_WRITE_ONLY)) {
        auto& ref = transform.ref();
        ref.cam_position = vec3f(0, 0, -10);
        ref.cam_view = vec3f(1, 1, 0);
        ref.cam_projection = make_perspective<float>(45.f, 1280.f / 768.f, 1.f, 100.f);
        ref.mv_matrix = make_translation<float>(0, 0, 2);
        transform.unmap();
    }

    loc = prog2->uniform_block_index("transform");
    transform.bind_point(loc);

    transform.release();

    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

}

static float angle = 0.f;
static float dx = 0.0f;
static float dy = 0.0f;

static float offset = 0.0f;
static int dir = 1;

void zap_example::update(double t, float dt) {
    transform.bind();
    if(transform.map(buffer_access::BA_WRITE_ONLY)) {
        transform.ref().mv_matrix = make_scale<float>(20,20,10) * make_translation<float>(-0.5f, -0.5f, 1);
        transform.unmap();
    }
    transform.release();

    auto fnc = [](float x, float y, float z) {
        vec3f v(x-0.5f, y-0.5f, z-0.5f);
        vec3f u(x-dx, y-dy, z-0.5f);
        return 0.3f/dot(v,v) + 0.2f/dot(u,u);
    };
    surf.clear();
    generators::surface<vertex_buffer_t>::marching_cubes(fnc, surf);

    dx = 0.5f + 0.2f*std::cos(2.f*angle) + 0.01f*offset;
    dy = 0.5f + 0.2f*std::sin(2.f*angle);

    vbuffer.bind();
    if(vbuffer.map(buffer_access::BA_WRITE_ONLY)) {
        std::copy(surf.begin(), surf.end(), vbuffer.begin());
        vbuffer.unmap();
    }
    vbuffer.release();

    angle += dt;
    angle = angle > 2*maths::PI ? 0 : angle;

    offset += dir*11*dt;
    if(offset < 2.f*PI) dir = 1;
    if(offset > 2.f*PI) dir = -1;
}

void zap_example::draw() {
    mesh.draw(primitive_type::PT_TRIANGLES, 0, surf.size());
    gl_error_check();
}

void zap_example::shutdown() {

}

int main(int argc, char* argv[]) {
    zap_example app;
    return app.run();
}
