//
// Created by Darren Otgaar on 2017/04/08.
//

#include <maths/io.hpp>
#include <tools/log.hpp>
#include <renderer/camera.hpp>
#include <loader/loader.hpp>
#include <engine/program.hpp>
#include <maths/transform.hpp>
#include <engine/state_stack.hpp>
#include "host/GLFW/application.hpp"

using namespace zap;
using namespace zap::maths;
using namespace zap::engine;
using namespace zap::renderer;

const char* const vtx_shdr = GLSL(
    in vec3 position;
    in vec3 normal;
    uniform mat4 mv;
    uniform mat4 pvm;

    out vec3 nor;

    void main() {
        nor = (mv * vec4(normal, 0.)).xyz;
        gl_Position = pvm * vec4(position, 1.);
    }
);

const char* const frg_shdr = GLSL(
    in vec3 nor;
    out vec4 frag_colour;

    void main() {
        frag_colour = vec4(max(.05, dot(nor, vec3(-1., 1., -1.))) * vec3(.5,.5,0), 1.);
    }
);

class models : public application {
public:
    models() : application("models", 1280, 768, false) { }

    bool initialise() override final;
    void update(double t, float dt) override final;
    void draw() override final;
    void shutdown() override final;

    void on_resize(int width, int height) override final;
    void on_mousemove(double x, double y) override final;
    void on_mousewheel(double xinc, double yinc) override final;

protected:
    camera cam_;
    loader loader_;
    std::vector<loader::mesh_p3n3t2_trii_t> meshes_;
    program prog_;
    state_stack rndr_state_;
};

bool models::initialise() {
    cam_.viewport(0, 0, 1280, 768);
    cam_.frustum(45.f, 1280.f/768.f, .5f, 1000.f);
    cam_.orthogonolise(vec3f(0,0,-1));

    if(!rndr_state_.initialise()) {
        LOG_ERR("Failed to initialise render state");
        return false;
    }

    rndr_state_.clear_colour(.15f, .15f, .15f, 1.f);

    LOG(LOG_BLUE, "Loading Meshes...");
#ifdef __APPLE__
    meshes_ = loader_.load("/Users/otgaard/Development/zap/assets/ducky.obj");
#else
    meshes_ = loader_.load("/home/otgaard/Development/zap/assets/ducky.obj");
#endif

    LOG("Meshes Returned:", meshes_.size());

    LOG(LOG_GREEN, "Building Shaders...");

    prog_.add_shader(shader_type::ST_VERTEX, vtx_shdr);
    prog_.add_shader(shader_type::ST_FRAGMENT, frg_shdr);

    if(!prog_.link()) {
        LOG_ERR("Failed to compile shader program");
        return false;
    }

    prog_.bind();


    return true;
}

void models::on_resize(int width, int height) {
}

void models::on_mousemove(double x, double y) {
}

void models::on_mousewheel(double xinc, double yinc) {
}

void models::update(double t, float dt) {
}

float angle = 0;

void models::draw() {
    transform4f transform;
    transform.translate(vec3f(0,-100,-700));
    transform.rotate(make_rotation(vec3f(0,1,0), angle));
    angle += 0.01f;

    prog_.bind_uniform("pvm", cam_.proj_view() * transform.gl_matrix());
    prog_.bind_uniform("mv", cam_.world_to_view() * transform.gl_matrix());

    for(auto& m : meshes_) {
        m.bind();
        m.draw();
        m.release();
    }
}

void models::shutdown() {
}

int main(int argc, char* argv[]) {
    models app;
    return app.run();
}
