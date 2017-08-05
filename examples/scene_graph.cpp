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
#include <engine/framebuffer.hpp>

#include <scene_graph/spatial.hpp>
#include <scene_graph/visual.hpp>
#include <scene_graph/node.hpp>

#include <graphics2/quad.hpp>
#include <graphics3/g3_types.hpp>
#include <generators/geometry/geometry3.hpp>
#include <generators/generator.hpp>
#include <generators/textures/planar.hpp>
#include <renderer/colour.hpp>
#include <renderer/render_context.hpp>

const char* const basic_vshdr = GLSL(
    uniform mat4 PVM;
    in vec3 position;
    out vec3 tex;
    void main() {
        tex = position;
        gl_Position = PVM * vec4(position, 1.);
    }
);

const char* const basic_fshdr = GLSL(
    uniform samplerCube diffuse_tex;
    uniform vec3 colour[4];

    in vec3 tex;
    out vec4 frag_colour;
    void main() {
        float h = 3*texture(diffuse_tex, tex).r;
        int A = int(floor(h));
        int B = min(A+1, 3);
        h = h - float(A);
        frag_colour = vec4(mix(colour[A], colour[B], h), 1.);
    }
);

const char* const skybox_fshdr = GLSL(
    in vec3 tex;
    out vec4 frag_colour;
    void main() {
        frag_colour = mix(vec4(.1, .2, .5, 1.), vec4(.3, .4, .7, 1.), dot(tex, vec3(0., 1., 0.)));
    }
);

// Adapted from Advanced Image Processing with DirectX 9 Pixel Shaders

const char* const blur_fshdr = GLSL(
    uniform sampler2D input_tex;
    uniform float blur_factor;
    uniform int direction;

    in vec2 tex;

    const float gaussian[5] = float[5](.0162162162, .0540540541, .1216216216, .1945945946, .2270270270);
    float disp[4] = float[4](1. * blur_factor, 2. * blur_factor, 3. * blur_factor, 4. * blur_factor);

    out vec4 frag_colour;

    void main() {
        frag_colour = vec4(0.);
        if(direction == 0) {    // Horizontal
            for(int i = 0; i != 4; ++i) frag_colour += texture(input_tex, vec2(tex.x - disp[3 - i], tex.y)) * gaussian[i];
            frag_colour += texture(input_tex, tex) * gaussian[4];
            for(int i = 0; i != 4; ++i) frag_colour += texture(input_tex, vec2(tex.x + disp[i], tex.y)) * gaussian[3 - i];
        } else {                // Vertical
            for(int i = 0; i != 4; ++i) frag_colour += texture(input_tex, vec2(tex.x, tex.y - disp[3 - i])) * gaussian[i];
            frag_colour += texture(input_tex, tex) * gaussian[4];
            for(int i = 0; i != 4; ++i) frag_colour += texture(input_tex, vec2(tex.x, tex.y + disp[i])) * gaussian[3 - i];
        }

        frag_colour.a = .95;
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

    void draw_scene();

protected:
    camera cam_;
    visual_t vis1_;
    vbuf_p3_t vbuf1_;
    ibuf_tri4_t ibuf1_;
    mesh_p3_tri4_t mesh1_;
    program prog1_;
    texture tex1_;
    sampler samp1_;

    texture tex2_;

    generator gen_;
    render_context context_;

    framebuffer fbuffer1_;
    framebuffer fbuffer2_;
    framebuffer fbuffer3_;
    quad quad1_;
    quad quad2_;
    sampler samp2_;

    int tex_idx = -1;
    int pvm_idx = -1;
    float inc = 0.f;

    // Skybox
    vbuf_p3_t skybox_vbuf_;
    ibuf_tri2_t skybox_ibuf_;
    mesh_p3_tri2_t skybox_mesh_;
    program skybox_prog_;
    render_context skybox_ctx_;
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

    auto sphere = generators::geometry3<vtx_p3_t, primitive_type::PT_TRIANGLES>::make_UVsphere<float, uint32_t>(100, 200, 1.f, false);
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

    render_task req{256, 256};
    req.scale.set(5.f, 5.f);
    req.project = render_task::projection::CUBE_MAP;
    auto pm = gen_.render_image<rgb888_t>(req).get();

    tex1_.set_type(texture_type::TT_CUBE_MAP);
    if(!tex1_.allocate() || !tex1_.initialise(pm, true)) {
        LOG_ERR("Failed to allocate or initialise texture");
        return false;
    }

    if(!samp1_.allocate()) {
        LOG_ERR("Failed to allocate sampler");
        return false;
    }

    req.scale.set(10.f, 10.f);
    pm = gen_.render_image<rgb888_t>(req).get();
    tex2_.set_type(texture_type::TT_CUBE_MAP);
    if(!tex2_.allocate() || !tex2_.initialise(pm,true)) {
        LOG_ERR("Failed to allocate or initialise texture");
        return false;
    }

    samp1_.initialise();
    samp1_.set_anisotropy(16.f);
    samp1_.set_min_filter(tex_filter::TF_LINEAR_MIPMAP_LINEAR);
    samp1_.set_mag_filter(tex_filter::TF_LINEAR);
    samp1_.set_wrap_s(tex_wrap::TW_MIRRORED_REPEAT);
    samp1_.set_wrap_t(tex_wrap::TW_MIRRORED_REPEAT);
    samp1_.set_wrap_r(tex_wrap::TW_MIRRORED_REPEAT);
    samp1_.release(0);

    context_.set_program(&prog1_);
    context_.add_sampler(&tex1_, &samp1_, &tex2_, &samp1_);
    context_.initialise();
    context_.set_parameter("colour[0]", {vec3f{0.f, 0.f, 0.f}, vec3f{1.f, 0.f, 0.f}, vec3f{1.f, 1.f, 0.f}, vec3f{1.f, 1.f, 1.f}});
    context_.set_texture_unit("diffuse_tex", 0);

    tex_idx = context_.get_index("diffuse_tex");
    pvm_idx = context_.get_index("PVM");

    if(!fbuffer1_.allocate() || !fbuffer1_.initialise(1, sc_width_/8, sc_height_/8, pixel_format::PF_RGBA, pixel_datatype::PD_UNSIGNED_BYTE, false, true)) {
        LOG_ERR("Failed to initialise the framebuffer");
        return false;
    }

    if(!fbuffer2_.allocate() || !fbuffer2_.initialise(1, sc_width_, sc_height_, pixel_format::PF_RGBA, pixel_datatype::PD_UNSIGNED_BYTE, false, false)) {
        LOG_ERR("Failed to initialise the framebuffer");
        return false;
    }

    if(!fbuffer3_.allocate() || !fbuffer3_.initialise(1, sc_width_, sc_height_, pixel_format::PF_RGBA, pixel_datatype::PD_UNSIGNED_BYTE, false, false)) {
        LOG_ERR("Failed to initialise the framebuffer");
        return false;
    }

    if(!quad1_.initialise(blur_fshdr) || !quad2_.initialise()) {
        LOG_ERR("Failed to initialise quad");
        return false;
    }

    quad2_.set_override(&fbuffer3_.get_attachment(0));

    quad1_.get_program()->bind();
    quad1_.get_program()->bind_uniform("blur_factor", 8.f/sc_width_);
    quad1_.get_program()->bind_uniform("direction", 0);
    quad1_.get_program()->bind_texture_unit("input_tex", 0);
    quad1_.get_program()->release();

    samp2_.allocate();
    samp2_.initialise();
    samp2_.set_min_filter(tex_filter::TF_LINEAR);
    samp2_.set_mag_filter(tex_filter::TF_LINEAR);
    samp2_.release(0);

    auto skybox = generators::geometry3<vtx_p3_t, primitive_type::PT_TRIANGLES>::make_skybox();
    if(!skybox_mesh_.allocate() || !skybox_vbuf_.allocate() || !skybox_ibuf_.allocate()) {
        LOG_ERR("Error building skybox");
        return false;
    }

    skybox_mesh_.bind(); skybox_vbuf_.bind(), skybox_ibuf_.bind();
    skybox_mesh_.set_stream(&skybox_vbuf_);
    skybox_mesh_.set_index(&skybox_ibuf_);

    if(!skybox_vbuf_.initialise(get<0>(skybox)) || !skybox_ibuf_.initialise(get<1>(skybox))) {
        LOG_ERR("Failed to initialise skybox");
        return false;
    }

    skybox_prog_.add_shader(shader_type::ST_VERTEX, basic_vshdr);
    skybox_prog_.add_shader(shader_type::ST_FRAGMENT, skybox_fshdr);
    if(!skybox_prog_.link()) {
        LOG_ERR("Failed to build skybox fragment shader");
        return false;
    }

    gl_error_check();

    alpha_blending(true);

    return true;
}

void scene_graph_test::on_resize(int width, int height) {
    cam_.frustum(45.f, width/float(height), .5f, 100.f);
    cam_.viewport(0, 0, width, height);
    cam_.frame(vec3f{0.f, 1.f, 0.f}, vec3f{0.f, 0.f, -1.f}, vec3f{0.f, 0.f, 2.f});
    quad1_.get_program()->bind();
    quad1_.get_program()->bind_uniform("blur_factor", 8.f/width);
    quad1_.get_program()->bind();
    fbuffer1_.initialise(1, width/8, height/8, pixel_format::PF_RGBA, pixel_datatype::PD_UNSIGNED_BYTE, false, true);
    fbuffer2_.initialise(1, width, height, pixel_format::PF_RGBA, pixel_datatype::PD_UNSIGNED_BYTE, false, false);
    fbuffer3_.initialise(1, width, height, pixel_format::PF_RGBA, pixel_datatype::PD_UNSIGNED_BYTE, false, false);
    quad1_.resize(width, height);
    quad2_.resize(width, height);
    gl_error_check();
}

void scene_graph_test::update(double t, float dt) {
    inc += dt;
    vec3f dir{cosf(inc), 0.f, sinf(inc)};
    cam_.orthogonolise(dir);
}

void scene_graph_test::draw_scene() {
    fbuffer1_.bind();
    skybox_prog_.bind();
    skybox_prog_.bind_uniform("PVM", cam_.proj_view()*make_scale(95.f, 95.f, 95.f));
    skybox_mesh_.bind();
    skybox_mesh_.draw();
    skybox_mesh_.release();
    skybox_prog_.release();

    mesh1_.bind();
    context_.bind();

    // Low frequency tex
    context_.set_parameter(pvm_idx, cam_.proj_view()
                                    * make_translation(-4.f, 0.f, -8.f)
                                    * make_rotation(vec3f{1.f, 0.f, 0.f}, PI/2)
                                    * make_rotation(vec3f{0.f, 0.f, -1.f}, inc));
    context_.set_texture_unit(tex_idx, 0);
    mesh1_.draw();

    // High frequency tex
    context_.set_parameter(pvm_idx, cam_.proj_view()
                                    * make_translation(+0.f, 0.f, -4.f)
                                    * make_rotation(vec3f{1.f, 0.f, 0.f}, PI/2)
                                    * make_rotation(vec3f{0.f, 0.f, 1.f}, inc));
    context_.set_texture_unit(tex_idx, 1);
    mesh1_.draw();

    fbuffer1_.release();
    context_.release();
    mesh1_.release();

    // Horizontal Pass
    fbuffer2_.bind();
    clear(0.f, 0.f, 0.f, 0.f);
    fbuffer1_.get_attachment(0).bind(0);
    samp2_.bind(0);
    quad1_.get_program()->bind();
    quad1_.get_program()->bind_uniform("direction", 0);
    quad1_.draw();
    samp2_.release(0);
    fbuffer1_.get_attachment(0).release();
    fbuffer2_.release();

    // Vertical Pass
    fbuffer3_.bind();
    clear(0.f, 0.f, 0.f, 0.f);
    fbuffer2_.get_attachment(0).bind(0);
    samp2_.bind(0);
    quad1_.get_program()->bind();
    quad1_.get_program()->bind_uniform("direction", 1);
    quad1_.draw();
    samp2_.release(0);
    fbuffer2_.get_attachment(0).release();
    fbuffer3_.release();

    mesh1_.bind();
    context_.bind();

    // Low frequency tex
    context_.set_parameter(pvm_idx, cam_.proj_view()
                                    * make_translation(-4.f, 0.f, -8.f)
                                    * make_rotation(vec3f{1.f, 0.f, 0.f}, PI/2)
                                    * make_rotation(vec3f{0.f, 0.f, -1.f}, inc));
    context_.set_texture_unit(tex_idx, 0);
    mesh1_.draw();

    // High frequency tex
    context_.set_parameter(pvm_idx, cam_.proj_view()
                                    * make_translation(+0.f, 0.f, -4.f)
                                    * make_rotation(vec3f{1.f, 0.f, 0.f}, PI/2)
                                    * make_rotation(vec3f{0.f, 0.f, 1.f}, inc));
    context_.set_texture_unit(tex_idx, 1);
    mesh1_.draw();

    context_.release();
    mesh1_.release();

    samp2_.bind(0);
    quad2_.draw();
    samp2_.release(0);

    gl_error_check();
}

void scene_graph_test::draw() {
    draw_scene();
}

void scene_graph_test::shutdown() {
}

int main(int argc, char* argv[]) {
    scene_graph_test app;
    return app.run();
}

// Generators