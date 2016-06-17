/* Created by Darren Otgaar on 2016/03/05. http://www.github.com/otgaard/zap */
#define LOGGING_ENABLED
#include <maths/io.hpp>
#include <tools/log.hpp>
#include "apps/application.hpp"
#include <engine/engine.hpp>

#include <engine/mesh.hpp>
#include <maths/algebra.hpp>
#include <engine/program.hpp>

#include "shader_src.hpp"

using namespace zap;
using namespace zap::maths;
using namespace zap::engine;

using p2_t = core::position<vec2s>;
using vtx_p2_t = vertex<p2_t>;
using vbuf_p2_t = vertex_buffer<vtx_p2_t, buffer_usage::BU_STATIC_DRAW>;
using mesh_p2_t = mesh<vertex_stream<vbuf_p2_t>, primitive_type::PT_TRIANGLE_FAN>;

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
};

void zap_example::initialise() {
    prog1.add_shader(new shader(shader_type::ST_VERTEX, vtx_src));
    prog1.add_shader(new shader(shader_type::ST_FRAGMENT, frg_src));
    if(!prog1.link(true)) {
        LOG_ERR("Couldn't link shader.");
        return;
    }

    mat4f proj_matrix = {
        2.f/sc_width_,            0.f, 0.f, -1.f,
                  0.f, 2.f/sc_height_, 0.f, -1.f,
                  0.f,            0.f, 2.f,  0.f,
                  0.f,            0.f, 0.f,  1.f
    };

    prog1.bind();
    auto loc = prog1.uniform_location("proj_matrix");
    prog1.bind_uniform(loc, proj_matrix);
    loc = prog1.uniform_location("mv_matrix");
    prog1.bind_uniform(loc, make_scale<float>(sc_width_, sc_height_, 1.f));
    loc = prog1.uniform_location("colour");
    prog1.bind_uniform(loc, vec3f(1,1,0));
    gl_error_check();

    if(!screenquad.allocate() || !sq_buffer.allocate()) {
        LOG_ERR("Could not allocate screenquad mesh or buffer.");
        return;
    }

    screenquad.set_stream(&sq_buffer);

    screenquad.bind();
    sq_buffer.bind();

    std::vector<vtx_p2_t> vertices = {
        {
            {{0, 0}}
        },
        {
            {{1, 0}}
        },
        {
            {{1,1}}
        },
        {
            {{0, 1}}
        }
    };

    sq_buffer.initialise(vertices);
    screenquad.release();
}

void zap_example::on_resize(int width, int height) {
    application::on_resize(width, height);

    mat4f proj_matrix = {
        2.f/width,        0.f, 0.f, -1.f,
              0.f, 2.f/height, 0.f, -1.f,
              0.f,        0.f, 2.f,  0.f,
              0.f,        0.f, 0.f,  1.f
    };

    prog1.bind();
    auto loc = prog1.uniform_location("proj_matrix");
    prog1.bind_uniform(loc, proj_matrix);
    loc = prog1.uniform_location("mv_matrix");
    prog1.bind_uniform(loc, make_scale<float>(width, height, 1));
    gl_error_check();
}

void zap_example::update(double t, float dt) {
}

void zap_example::draw() {
    screenquad.bind();
    screenquad.draw();
    screenquad.release();
}

void zap_example::shutdown() {
}

int main(int argc, char* argv[]) {
    zap_example app;
    return app.run();
}
