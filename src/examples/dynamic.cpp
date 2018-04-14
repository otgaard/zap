//
// Created by Darren Otgaar on 2017/08/19.
//

#include <array>
#include <tools/log.hpp>
#include <maths/algebra.hpp>
#include <host/GLFW/application.hpp>
#include <engine/vertex_buffer.hpp>
#include <engine/index_buffer.hpp>
#include <engine/accessor.hpp>
#include <engine/program.hpp>
#include <graphics/graphics3/g3_types.hpp>
#include <graphics/loader/obj_loader.hpp>
#include <engine/state_stack.hpp>

using namespace zap;
using namespace zap::core;
using namespace zap::maths;
using namespace zap::engine;
using namespace zap::graphics;

// Provides examples of how to use buffers, accessors, and render batches in conjunction with models and procedurally
// generated geometry

const char* const static_prog_vshdr = GLSL(
    uniform mat4 MVP;
    uniform mat4 MV;

    in vec3 position;
    in vec3 normal;
    in vec2 texcoord1;

    out vec2 tex;
    out vec3 nor;

    void main() {
        gl_Position = MVP * vec4(position, 1.);
        nor = (MV * vec4(normal, 0.)).xyz;
        tex = texcoord1;
    }
);

const char* const static_prog_fshdr = GLSL(
    uniform vec4 colour;
    uniform vec3 light_dir;

    in vec2 tex;
    in vec3 nor;

    out vec4 frag_colour;
    void main() {
        float s = max(dot(normalize(nor), light_dir), 0.);
        frag_colour = s * colour;
    }
);

const size_t STATIC_VCOUNT = 100000;
const size_t STATIC_ICOUNT = 3*STATIC_VCOUNT;
const size_t STREAM_VCOUNT = 100000;
const size_t STREAM_ICOUNT = 3*STREAM_VCOUNT;

class dynamic_app : public application {
public:
    dynamic_app() : application{"dynamic_app", 1024, 768} { }

    bool initialise() final;
    void update(double t, float dt) final;
    void draw() final;
    void shutdown() final;

    void on_resize(int width, int height) final;

private:
    std::array<range, 10> objects;
    mesh_p3n3t2_u32_t static_mesh_;
    mesh_p3c4t2_u32_t stream_mesh_;
    mesh_p3c4_t2_u32_t instanced_mesh_;
    program static_prog_;

    mesh_p3n3t2_u32_t bunny_;
    mat4f projMatrix_;
    mat4f viewMatrix_;

    state_stack rndr_state_;
};

bool dynamic_app::initialise() {
    bunny_ = obj_loader::load_mesh("/Users/otgaard/Development/zap/assets/models/bunny.obj");

    static_mesh_ = make_mesh<vtx_p3n3t2_t, uint32_t>(STATIC_VCOUNT, STATIC_ICOUNT);
    stream_mesh_ = make_mesh<vtx_p3c4t2_t, uint32_t>(STREAM_VCOUNT, STREAM_ICOUNT);

    if(!static_mesh_.is_allocated() || !stream_mesh_.is_allocated()) {
        LOG_ERR("Failed to allocate meshes");
        return false;
    }

    static_prog_.add_shader(shader_type::ST_VERTEX, static_prog_vshdr);
    static_prog_.add_shader(shader_type::ST_FRAGMENT, static_prog_fshdr);
    if(!static_prog_.link()) {
        LOG_ERR("Failed to link static_prog");
        return false;
    }

    if(!rndr_state_.initialise()) {
        LOG_ERR("Failed to initialise render state");
        return false;
    }

    return true;
}

void dynamic_app::update(double t, float dt) {
    //static_prog_.bind();
    //static_prog_.bind_uniform("MVP", projMatrix_ * viewMatrix_ * make_scale(.5f, .5f, .5f) * make_rotation(vec3f{0.f, 1.f, 0.f}, ));
    //static_prog_.bind_uniform("MV", viewMatrix_);
    //static_prog_.release();
}

void dynamic_app::draw() {
    rndr_state_.clear(0.f, 0.f, 0.f, 1.f);
    static_prog_.bind();
    bunny_.bind();
    bunny_.draw(primitive_type::PT_TRIANGLES);
    bunny_.release();
    static_prog_.release();
}

void dynamic_app::shutdown() {
}

void dynamic_app::on_resize(int width, int height) {
    projMatrix_ = make_perspective(40.f, float(width)/height, .5f, 100.f);
    viewMatrix_ = look_at(vec3f{0.f, 0.f, 0.f}, vec3f{0.f, 2.f, 8.f});
    static_prog_.bind();
    static_prog_.bind_uniform("MVP", projMatrix_ * viewMatrix_ * make_scale(.2f, .2f, .2f));
    static_prog_.bind_uniform("MV", viewMatrix_);
    static_prog_.bind_uniform("light_dir", normalise(vec3f{1.f, 1.f, 1.f}));
    static_prog_.bind_uniform("colour", vec4f{1.f, 1.f, 0.f, 1.f});
    static_prog_.release();
}

int main(int argc, char* argv[]) {
    dynamic_app app{};
    return app.run();
}
