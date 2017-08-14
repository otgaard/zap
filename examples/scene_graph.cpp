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
#include <renderer/light.hpp>

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
using p3n3t2_geo3_tri = generators::geometry3<vtx_p3n3t2_t, primitive_type::PT_TRIANGLES>;
using p3n3t2_geo3_ts = generators::geometry3<vtx_p3n3t2_t, primitive_type::PT_TRIANGLE_STRIP>;
using p3n3t2_geo3_tf = generators::geometry3<vtx_p3n3t2_t, primitive_type::PT_TRIANGLE_FAN>;

const char* const basic_vshdr = GLSL(
    layout (std140) uniform light_dir_basic {
        vec4 light_dir;
        float light_intensity;
        float light_exponent;
    };

    uniform mat4 PVM;
    uniform mat3 normal_matrix;
    in vec3 position;
    in vec3 normal;
    in vec2 texcoord1;
    out float lD;
    out vec2 tex2;
    void main() {
        vec3 N = normal_matrix * normal;
        lD = light_intensity * max(dot(N, light_dir.xyz), 0.);
        tex2 = texcoord1;
        gl_Position = PVM * vec4(position, 1.);
    }
);

const char* const basic_fshdr = GLSL(
    uniform sampler2D diffuse_tex;
    in float lD;
    in vec2 tex2;
    out vec4 frag_colour;
    void main() {
        frag_colour = lD * texture(diffuse_tex, tex2);
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
    generator gen_;
    std::vector<visual_t> visuals_;
    zap::renderer::renderer rndr_;
    std::vector<std::unique_ptr<mesh_base>> meshes_;
    std::unique_ptr<render_context> context_;
    std::vector<texture> textures_;
    std::vector<sampler> samplers_;
    std::vector<dir_light> dir_lights_;
    uniform_buffer<dir_light::block_t> lights_;
};

bool scene_graph_test::initialise() {
    clear(0.f, 0.f, 0.f, 0.f);

    if(!gen_.initialise()) {
        LOG_ERR("Failed to initialise generator or renderer");
        return false;
    }

    // Setup the shared context (reuse same one until Context supports instances)
    context_ = std::unique_ptr<render_context>(new render_context{basic_vshdr, basic_fshdr});

    // Create some textures
    render_task req{256, 128, render_task::basis_function::USER_FUNCTION};
    req.mipmaps = true;

    req.scale.set(20.f, 20.f);
    textures_.emplace_back(gen_.render_spherical(req, [](float x, float y, float z, generator& gen) {
        int ix = maths::floor(x), iy = maths::floor(y), iz = maths::floor(z);
        float value = clamp(.707f + .5f*gen.pnoise(x - ix, y - iy, z - iz, ix, iy, iz));
        vec3b colour = lerp(value, vec3b(255, 0, 0), vec3b(255, 255, 0));
        return rgb888_t{colour};
    }));

    req.scale.set(2.f, 2.f);
    textures_.emplace_back(gen_.render_spherical(req, [](float x, float y, float z, generator& gen) {
        float lat = .2f*TWO_PI<float>*std::acos(y/2.f);
        float lon = .4f*TWO_PI<float>*std::atan(x/z);
        int ilat = maths::floor(lat), ilon = maths::floor(lon);
        bool parity = ((ilat+1)+ilon)%2 == 0;
        return rgb888_t{parity ? vec3b{255, 0, 0} : vec3b{0, 0, 255}};
    }));

    req.scale.set(20.f, 20.f);
    textures_.emplace_back(gen_.render_cylindrical(req, [](float x, float y, float z, generator& gen) {
        int ix = maths::floor(x), iy = maths::floor(y), iz = maths::floor(z);
        float value = clamp(.707f + .5f*gen.vnoise(x - ix, y - iy, z - iz, ix, iy, iz));
        vec3b colour = lerp(value, vec3b(0, 255, 0), vec3b(128, 128, 64));
        return rgb888_t{colour};
    }));

    req.width = 512; req.height = 512;
    req.scale.set(1.f, 1.f);
    textures_.emplace_back(gen_.render_planar(req, [](float x, float y, generator& gen) {
        int ix = maths::floor(100*x), iy = maths::floor(100*y);
        byte c = ((ix+1)+iy) % 2 ? 0 : 255;
        return rgb888_t{ c, c, c };
    }));

    samplers_.emplace_back();
    samplers_[0].allocate();
    samplers_[0].initialise();
    samplers_[0].set_mag_filter(tex_filter::TF_LINEAR);
    samplers_[0].set_min_filter(tex_filter::TF_LINEAR_MIPMAP_LINEAR);

    samplers_.emplace_back();
    samplers_[1].allocate();
    samplers_[1].initialise();
    samplers_[1].set_anisotropy(16.f);
    samplers_[1].set_mag_filter(tex_filter::TF_NEAREST);
    samplers_[1].set_min_filter(tex_filter::TF_LINEAR_MIPMAP_LINEAR);

    // Warning: Don't realloc textures or samplers after this...
    context_->add_sampler(&textures_[0], &samplers_[0],
                          &textures_[1], &samplers_[0],
                          &textures_[2], &samplers_[0],
                          &textures_[3], &samplers_[1]);

    if(!context_->initialise()) {
        LOG_ERR("Failed to initialise render_context");
        return false;
    }

    // Add lights
    dir_lights_.emplace_back();
    dir_lights_[0].block.light_dir.set(0.f, 10.f, -10.f, 0.f);
    dir_lights_[0].block.light_dir.normalise();
    dir_lights_[0].block.light_intensity = 1.f;
    dir_lights_[0].block.light_exponent = .5f;

    LOG(dir_lights_[0].block.light_dir, sizeof(dir_lights_[0].block));

    lights_.allocate();
    lights_.bind();
    lights_.initialise(dir_lights_[0].block);
    lights_.release();
    context_->add_uniform_buffer("light_dir_basic", &lights_);

    auto quad_mesh = p3n3t2_geo3_tf::make_mesh(p3n3t2_geo3_tf::make_quad(200.f, 200.f));
    visual_t quad{quad_mesh.get(), context_.get()};
    meshes_.emplace_back(std::move(quad_mesh));

    quad.rotate(make_rotation(vec3f{1.f, 0.f, 0.f}, PI<float>/2.f));
    visuals_.push_back(quad);

    auto sphere_mesh = p3n3t2_geo3_tri::make_mesh(p3n3t2_geo3_tri::make_UVsphere<float,uint32_t>(30, 60, 1.f, false));
    visual_t sphere{sphere_mesh.get(), context_.get()};
    meshes_.emplace_back(std::move(sphere_mesh));

    sphere.translate(vec3f{-7.f, 4.f, -14.f});
    visuals_.push_back(sphere);
    sphere.translate(vec3f{+0.f, 4.f, -14.f});
    visuals_.push_back(sphere);
    sphere.translate(vec3f{+7.f, 4.f, -14.f});
    visuals_.push_back(sphere);

    auto cylinder_mesh = p3n3t2_geo3_ts::make_mesh(p3n3t2_geo3_ts::make_cylinder(5, 30, 1.f, .5f, false));
    visual_t cylinder{cylinder_mesh.get(), context_.get()};
    meshes_.emplace_back(std::move(cylinder_mesh));

    cylinder.translate(vec3f{-3.f, 1.f, -2.f});
    visuals_.push_back(cylinder);
    cylinder.translate(vec3f{+0.f, 1.f, -2.f});
    visuals_.push_back(cylinder);
    cylinder.translate(vec3f{+3.f, 1.f, -2.f});
    visuals_.push_back(cylinder);

    gl_error_check();
    return true;
}

void scene_graph_test::on_resize(int width, int height) {
    cam_.viewport(0, 0, width, height);
    cam_.world_pos(vec3f{0.f, 2.f, 10.f});
    cam_.look_at(vec3f{0.f, 0.f, -100.f});
    cam_.frustum(45.f, width/float(height), .5f, 100.f);

    const vec3f lD = normalise(vec3f{0.f, 10.f, 10.f});
    const vec3f vD = cam_.world_to_view() * lD;
    dir_lights_[0].block.light_dir.set(vD.x, vD.y, vD.z, 0.);
    lights_.bind();
    lights_.initialise(dir_lights_[0].block);
    lights_.release();


    gl_error_check();
}

void scene_graph_test::update(double t, float dt) {
    static float inc = 0.f;
    auto rot = make_rotation(vec3f{0.f, 1.f, 0.f}, inc) * make_rotation(vec3f{1.f, 0.f, 0.f}, PI<float>/2);
    for(int i = 1; i != 7; ++i) visuals_[i].rotate(rot);
    inc += dt;
    gl_error_check();
}

void scene_graph_test::draw() {
    for(int i = 0; i != 7; ++i) {
        context_->set_parameter("PVM", cam_.proj_view() * visuals_[i].world_transform().gl_matrix());
        context_->set_parameter("normal_matrix", (cam_.world_to_view() * visuals_[i].world_transform().gl_matrix()).inverse().transpose().rotation());
        context_->set_texture_unit("diffuse_tex", i == 0 ? 3 : (i-1)%3);
        visuals_[i].draw(rndr_);
    }
    gl_error_check();
}

void scene_graph_test::shutdown() {
    gl_error_check();
}

int main(int argc, char* argv[]) {
    scene_graph_test app;
    return app.run();
}
