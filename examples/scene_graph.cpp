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
    in vec2 texcoord1;
    out vec2 tex2;
    out vec3 tex3;
    void main() {
        tex2 = texcoord1;
        tex3 = normal;
        gl_Position = PVM * vec4(position, 1.);
    }
);

const char* const basic_fshdr = GLSL(
    uniform sampler2D diffuse_tex;
    in vec2 tex2;
    out vec4 frag_colour;
    void main() {
        frag_colour = texture(diffuse_tex, tex2);
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
    std::vector<visual_t> visuals_;
    zap::renderer::renderer rndr_;
    generator gen_;
    std::vector<std::unique_ptr<mesh_base>> meshes_;
    std::unique_ptr<render_context> context_;
    std::vector<texture> textures_;
};

bool scene_graph_test::initialise() {
    clear(0.f, 0.f, 0.f, 0.f);

    if(!gen_.initialise()) {
        LOG_ERR("Failed to initialise generator");
        return false;
    }

    // Setup the shared context (reuse same one until Context supports instances)
    context_ = std::make_unique<render_context>(basic_vshdr, basic_fshdr);
    render_task req{512, 256, render_task::basis_function::USER_FUNCTION};
    req.scale.set(10.f, 10.f);

    textures_.emplace_back(gen_.render_spherical(req, [](float x, float y, float z, generator& gen) {
        int ix = maths::floor(x), iy = maths::floor(y), iz = maths::floor(z);
        float value = clamp(.707f + .5f*gen.pnoise(x - ix, y - iy, z - iz, ix, iy, iz));
        vec3b colour = lerp(value, vec3b(255, 0, 0), vec3b(255, 255, 0));
        return rgb888_t{colour};
    }));

    textures_.emplace_back(gen_.render_spherical(req, [](float x, float y, float z, generator& gen) {
        float lat = TWO_PI<float>*acosf(y/10.f);
        float lon = TWO_PI<float>*atanf(x/z);
        int ilat = maths::floor(lat), ilon = maths::floor(lon);
        bool parity = ((ilat+1)+ilon)%2 == 0;
        return rgb888_t{parity ? vec3b{255,0,0} : vec3b{0,0,255}};
    }));

    context_->add_texture(&textures_.back());
    if(!context_->initialise()) {
        LOG_ERR("Failed to initialise render_context");
        return false;
    }

    auto sphere_mesh = p3n3t2_geo3::make_mesh(p3n3t2_geo3::make_UVsphere(30, 60, 1.f, false));
    visual_t sphere{sphere_mesh.get(), context_.get()};
    sphere.set_mesh(sphere_mesh.get());
    meshes_.emplace_back(std::move(sphere_mesh));
    visuals_.emplace_back(std::move(sphere));
    gl_error_check();
    return true;
}

void scene_graph_test::on_resize(int width, int height) {
    cam_.viewport(0, 0, width, height);
    cam_.world_pos(vec3f{0.f, 2.f, 5.f});
    cam_.look_at(vec3f{0.f, 0.f, 0.f});
    cam_.frustum(45.f, width/float(height), .5f, 100.f);
    gl_error_check();
}

void scene_graph_test::update(double t, float dt) {
    static float inc = 0.f;
    visuals_[0].rotate(make_rotation(vec3f{0.f, 1.f, 0.f}, inc) * make_rotation(vec3f{1.f, 0.f, 0.f}, PI<float>/2));
    inc += dt;
    gl_error_check();
}

void scene_graph_test::draw() {
    context_->set_parameter("PVM", cam_.proj_view() * visuals_[0].world_transform().gl_matrix());
    visuals_[0].draw(rndr_);
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