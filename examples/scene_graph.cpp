//
// Created by Darren Otgaar on 2017/07/27.
//

/**
 * Tests the scene graph classes
 */

#include <tools/log.hpp>
#include <maths/io.hpp>
#include <host/GLFW/application.hpp>

#include <maths/geometry/sphere.hpp>

#include <renderer/camera.hpp>
#include <engine/sampler.hpp>

#include <scene_graph/spatial.hpp>
#include <scene_graph/visual.hpp>
#include <scene_graph/node.hpp>

#include <graphics3/g3_types.hpp>
#include <generators/geometry/geometry3.hpp>
#include <generators/generator.hpp>

const char* const basic_vshdr = GLSL(
    uniform mat4 PVM;
    in vec3 position;
    in vec2 texcoord1;
    out vec2 tex;
    void main() {
        tex = texcoord1;
        gl_Position = PVM * vec4(position, 1.);
    }
);

const char* const basic_fshdr = GLSL(
    uniform sampler2D diffuse_tex;
    in vec2 tex;
    out vec4 frag_colour;
    void main() {
        frag_colour = texture(diffuse_tex, tex);
    }
);

using namespace zap;
using namespace zap::maths;
using namespace zap::renderer;
using namespace zap::graphics;
using namespace zap::scene_graph;

using spheref = geometry::sphere<float>;
using spatial_t = spatial<transform4f, spheref>;
using node_t = node<spatial_t>;
using visual_t = visual<spatial_t>;

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
    visual_t vis1_;
    vbuf_p3t2_t vbuf1_;
    ibuf_tri4_t ibuf1_;
    mesh_p3t2_trii_t mesh1_;
    program prog1_;
    texture tex1_;
    sampler samp1_;
    generator gen_;
};

bool scene_graph_test::initialise() {
    clear(0.f, 0.f, 0.f, 0.f);

    if(!vbuf1_.allocate() || !ibuf1_.allocate() || !mesh1_.allocate()) {
        LOG_ERR("Failed to initialise vbuf or mesh");
        return false;
    }

    mesh1_.bind(); vbuf1_.bind(); ibuf1_.bind();
    mesh1_.set_stream(&vbuf1_);
    mesh1_.set_index(&ibuf1_);

    auto sphere = generators::geometry3<vtx_p3t2_t, primitive_type::PT_TRIANGLES>::make_UVsphere<float, uint32_t>(30, 60, 1.f, false);
    if(!vbuf1_.initialise(get<0>(sphere))) {
        LOG_ERR("Failed to initialise sphere");
        return false;
    }

    if(!ibuf1_.initialise(get<1>(sphere))) {
        LOG_ERR("Failed to initialise sphere");
        return false;
    }

    prog1_.add_shader(shader_type::ST_VERTEX, basic_vshdr);
    prog1_.add_shader(shader_type::ST_FRAGMENT, basic_fshdr);
    if(!prog1_.link()) {
        LOG_ERR("Failed to compile shader program");
        return false;
    }

    if(!gen_.initialise()) {
        LOG_ERR("Noise Generator Initialisation Failed");
        return false;
    }

    render_task req{512, 256};
    req.scale.set(10.f, 1.f);
    req.project = render_task::projection::SPHERICAL;
    auto pm = gen_.render_image<rgb888_t>(req).get();

    if(!tex1_.allocate() || !tex1_.initialise(pm, true)) {
        LOG_ERR("Failed to allocate or initialise texture");
        return false;
    }

    if(!samp1_.allocate()) {
        LOG_ERR("Failed to allocate sampler");
        return false;
    }

    samp1_.initialise();
    samp1_.set_anisotropy(16.f);
    samp1_.set_min_filter(tex_filter::TF_LINEAR_MIPMAP_LINEAR);
    samp1_.set_mag_filter(tex_filter::TF_LINEAR);
    samp1_.release(0);

    //wire_frame(true);
    //bf_culling(false);

    gl_error_check();

    return true;
}

void scene_graph_test::on_resize(int width, int height) {
    cam_.frustum(45.f, float(width)/height, .5f, 100.f);
    cam_.viewport(0, 0, width, height);
    cam_.frame(vec3f{0.f, 1.f, 0.f}, vec3f{0.f, 0.f, -1.f}, vec3f{0.f, 0.f, 2.f});

    prog1_.bind();
    prog1_.bind_uniform("PVM", cam_.proj_view());
    prog1_.bind_texture_unit("diffuse_tex", 0);
    prog1_.release();

    gl_error_check();
}

float inc = 0;
void scene_graph_test::update(double t, float dt) {
    inc += dt;
}

void scene_graph_test::draw() {
    prog1_.bind();

    prog1_.bind_uniform("PVM", cam_.proj_view() * make_translation(0.f, 0.f, -2.f) * make_rotation(vec3f{1.f, 0.f, 0.f}, PI/2) *
                        make_rotation(vec3f{0.f, 0.f, 1.f}, inc));

    mesh1_.bind();
    tex1_.bind(0);
    samp1_.bind(0);
    mesh1_.draw();
    samp1_.release(0);
    tex1_.release();
    mesh1_.release();

    prog1_.release();
}

void scene_graph_test::shutdown() {
}

int main(int argc, char* argv[]) {
    scene_graph_test app;
    return app.run();
}

// Generators