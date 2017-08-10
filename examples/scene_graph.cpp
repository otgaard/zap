//
// Created by Darren Otgaar on 2017/07/27.
//

/**
 * Tests the scene graph classes
 */

#include <tools/log.hpp>
#include <maths/io.hpp>
#include <host/GLFW/application.hpp>

#include <scene_graph/node.hpp>
#include <scene_graph/visual.hpp>

#include <renderer/camera.hpp>
#include <renderer/renderer.hpp>
#include <generators/generator.hpp>
#include <generators/geometry/geometry3.hpp>

using namespace zap;
using namespace zap::maths;
using namespace zap::engine;
using namespace zap::graphics;
using namespace zap::renderer;
using namespace zap::scene_graph;

using spheref = geometry::sphere<float>;
using spatial_t = spatial<transform4f, spheref>;
using node_t = node<spatial_t>;
using visual_t = visual<spatial_t>;
using p3n3t2_geo3 = generators::geometry3<vtx_p3n3t2_t, primitive_type::PT_TRIANGLES>;

const char* const basic_vshdr = GLSL(
    uniform mat4 PVM;
    in vec3 position;
    in vec3 normal;
    out vec3 tex;
    void main() {
        tex = normal;
        gl_Position = PVM * vec4(position, 1.);
    }
);

const char* const basic_fshdr = GLSL(
    uniform samplerCube diffuse_tex;
    in vec3 tex;
    out vec4 frag_colour;
    void main() {
        frag_colour = texture(diffuse_tex, tex);
    }
);

class scene_graph_test : public application {
public:
    scene_graph_test() : application{"scene_graph_test", 1280, 900, false}, cam_{true} { }

    bool initialise() final;
    void update(double t, float dt) final;
    void draw() final;
    void shutdown() final;

    void on_resize(int width, int height) final;

protected:
    camera cam_;
    visual_t sphere_;
    zap::renderer::renderer rndr_;
    generator gen_;
    std::vector<std::unique_ptr<mesh_base>> meshes_;
    std::vector<std::unique_ptr<render_context>> contexts_;
    std::vector<texture> textures_;
};

bool scene_graph_test::initialise() {
    clear(0.f, 0.f, 0.f, 0.f);

    if(!gen_.initialise()) {
        LOG_ERR("Failed to initialise generator");
        return false;
    }

    auto sphere_mesh = p3n3t2_geo3::make_mesh(p3n3t2_geo3::make_UVsphere(30, 60, 1.f, false));
    sphere_.set_mesh(sphere_mesh.get());
    meshes_.emplace_back(std::move(sphere_mesh));

    contexts_.emplace_back(new render_context(basic_vshdr, basic_fshdr));

    render_task req{1024, 1024, render_task::basis_function::FUNCTION};
    req.scale.set(255.f, 255.f);
    textures_.emplace_back(gen_.render_cubemap(req, [](float x, float y, float z, generator& gen) {
        return rgb888_t{abs(x), abs(y), abs(z)};
    }));
    contexts_.back()->add_texture(&textures_.back());

    if(!contexts_.back()->initialise()) {
        LOG_ERR("Failed to initialise render_context");
        return false;
    }

    sphere_.set_context(contexts_.back().get());

    gl_error_check();
    return true;
}

void scene_graph_test::on_resize(int width, int height) {
    cam_.viewport(0, 0, width, height);
    cam_.world_pos(vec3f{0.f, 0.f, 5.f});
    cam_.look_at(vec3f{0.f, 0.f, 0.f});
    cam_.frustum(45.f, width/float(height), .5f, 100.f);
    contexts_.back()->set_parameter("PVM", cam_.proj_view() * sphere_.world_transform().gl_matrix());
    gl_error_check();
}

void scene_graph_test::update(double t, float dt) {
    gl_error_check();
}

void scene_graph_test::draw() {
    sphere_.draw(rndr_);
    gl_error_check();
}

void scene_graph_test::shutdown() {
    gl_error_check();
}

int main(int argc, char* argv[]) {
    scene_graph_test app;
    return app.run();
}

// Generators