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

#include <renderer/scene_graph/spatial.hpp>
#include <renderer/scene_graph/visual.hpp>
#include <renderer/scene_graph/node.hpp>

#include <graphics/graphics2/quad.hpp>
#include <graphics/graphics3/g3_types.hpp>
#include <graphics/generators/geometry/geometry3.hpp>
#include <graphics/generators/generator.hpp>
#include <graphics/generators/textures/planar.hpp>
#include <graphics/colour.hpp>
#include <engine/state_stack.hpp>
#include <renderer/renderer.hpp>

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
            frag_colour = vec4(mix(colour[A], colour[B], h), .4);
        }
);

const char* const skybox_fshdr = GLSL(
        const int MASK = 0xFF;
        uniform usampler1D perm;
        uniform sampler1D grad;

        in vec3 tex;

        out vec4 frag_colour;

        int perm1(int x) { return int(texelFetch(perm, x & MASK, 0).r); }
        int perm2(int x, int y) { return int(texelFetch(perm, (x + perm1(y)) & MASK, 0).r); }
        int perm3(int x, int y, int z) { return int(texelFetch(perm, (x + perm2(y, z)) & MASK, 0).r); }
        float grad1(int x) { return texelFetch(grad, perm1(x), 0).r; }
        float grad2(int x, int y) { return texelFetch(grad, perm2(x, y), 0).r; }
        float grad3(int x, int y, int z) { return texelFetch(grad, perm3(x, y, z), 0).r; }

        void main() {
            float theta = dot(tex, vec3(0., 1., 0));
            vec3 pos = 300. * normalize(tex);
            vec3 fpos = floor(pos) + vec3(.5);
            ivec3 ipos = ivec3(int(fpos.x), int(fpos.y), int(fpos.z));
            float d = .5 - length(fpos - pos);
            int threshold = perm3(ipos.x, ipos.y, ipos.z);
            frag_colour = threshold > 253
                          ? vec4(d * (1.25 + .5*grad1(ipos.x)),
                                 d * (1.25 + .5*grad1(ipos.y)),
                                 d * (1.25 + .5*grad1(ipos.z)),
                                 1.)
                          : mix(vec4(.0, .0, .0, 1.), vec4(.0, .0, .0, 1.), .5 + .5*theta);
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
        }
);

// Blend to images (glow post-processing)
const char* const blend_fshdr = GLSL(
        uniform sampler2D texA;
        uniform sampler2D texB;

        in vec2 tex;

        out vec4 frag_colour;

        void main() {
            vec4 colA = texture(texA, tex);
            vec4 colB = texture(texB, tex);
            frag_colour = min((colA + colB) - colA*colB, 1.);
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

class blur_glow : public application {
public:
    blur_glow() : application{"blur_glow", 2*1280, 2*900}, cam_{true} { }

    bool initialise() final;
    void update(double t, float dt) final;
    void draw() final;
    void shutdown() final;

    void on_resize(int width, int height) final;

    void draw_scene();

protected:
    camera cam_;
    std::unique_ptr<mesh_p3_u32_t> sphere_;
    program planet_prog_;
    texture planet_tex1_;
    sampler samp1_;

    texture planet_tex2_;

    generator gen_;
    render_context planet_ctx_;

    framebuffer fbuffer1_;
    framebuffer fbuffer2_;
    framebuffer fbuffer3_;
    quad blur_quad_;
    sampler samp2_;

    int tex_idx = -1;
    int pvm_idx = -1;
    float inc = 0.f;

    // Skybox
    std::unique_ptr<mesh_p3_u16_t> skybox_;
    program skybox_prog_;
    render_context skybox_ctx_;

    quad output_quad_;   // The final output (the blend shader)

    state_stack stack;
    zap::renderer::renderer rndr_;

    render_state blend_state_;
    render_state depth_state_;
};

bool blur_glow::initialise() {
    if(!zap::engine::init()) {
        LOG_ERR("Failed to initialise the zap engine");
        return false;
    }

    rndr_.get_state_stack()->clear_colour(0.f, 0.f, 0.f, 0.f);
    rndr_.initialise();

    sphere_ = make_mesh(generators::geometry3<vtx_p3_t, primitive_type::PT_TRIANGLES>::make_UVsphere<float, uint32_t>(100, 200, 1.f, false));

    if(!planet_prog_.link(basic_vshdr, basic_fshdr)) {
        LOG_ERR("Failed to compile shader program");
        return false;
    }

    if(!gen_.initialise()) {
        LOG_ERR("Noise Generator Initialisation Failed");
        return false;
    }

    render_task req{256, 256};
    req.scale.set(15.f, 15.f);
    req.project = render_task::projection::CUBE_MAP;
    auto pm = gen_.render_image<rgb888_t>(req).get();

    planet_tex1_.set_type(texture_type::TT_CUBE_MAP);
    if(!planet_tex1_.allocate() || !planet_tex1_.initialise(pm, true)) {
        LOG_ERR("Failed to allocate or initialise texture");
        return false;
    }

    if(!samp1_.allocate()) {
        LOG_ERR("Failed to allocate sampler");
        return false;
    }

    req.scale.set(30.f, 30.f);
    pm = gen_.render_image<rgb888_t>(req).get();
    planet_tex2_.set_type(texture_type::TT_CUBE_MAP);
    if(!planet_tex2_.allocate() || !planet_tex2_.initialise(pm,true)) {
        LOG_ERR("Failed to allocate or initialise texture");
        return false;
    }

    samp1_.initialise();
    samp1_.set_anisotropy(16.f);
    samp1_.set_min_filter(tex_filter::TF_LINEAR_MIPMAP_LINEAR);
    samp1_.set_mag_filter(tex_filter::TF_LINEAR);
    samp1_.set_wrap_s(tex_wrap::TW_CLAMP_TO_EDGE);
    samp1_.set_wrap_t(tex_wrap::TW_CLAMP_TO_EDGE);
    samp1_.set_wrap_r(tex_wrap::TW_CLAMP_TO_EDGE);
    samp1_.release(0);

    planet_ctx_.set_program(&planet_prog_);
    planet_ctx_.add_sampler(&planet_tex1_, &samp1_, &planet_tex2_, &samp1_);
    planet_ctx_.initialise();
    planet_ctx_.set_parameter("colour[0]", {vec3f{0.f, 0.f, 0.f}, vec3f{1.f, 0.f, 0.f}, vec3f{1.f, 1.f, 0.f}, vec3f{1.f, 1.f, 1.f}});
    planet_ctx_.set_texture_unit("diffuse_tex", 0);

    tex_idx = planet_ctx_.get_index("diffuse_tex");
    pvm_idx = planet_ctx_.get_index("PVM");

    if(!fbuffer1_.allocate() || !fbuffer1_.initialise(1, sc_width_/8, sc_height_/8, pixel_format::PF_RGBA, pixel_datatype::PD_UNSIGNED_BYTE, false, false)) {
        LOG_ERR("Failed to initialise the framebuffer");
        return false;
    }

    if(!fbuffer2_.allocate() || !fbuffer2_.initialise(1, sc_width_, sc_height_, pixel_format::PF_RGBA, pixel_datatype::PD_UNSIGNED_BYTE, false, true)) {
        LOG_ERR("Failed to initialise the framebuffer");
        return false;
    }

    if(!fbuffer3_.allocate() || !fbuffer3_.initialise(1, sc_width_, sc_height_, pixel_format::PF_RGBA, pixel_datatype::PD_UNSIGNED_BYTE, false, true)) {
        LOG_ERR("Failed to initialise the framebuffer");
        return false;
    }

    if(!blur_quad_.initialise(blur_fshdr) || !output_quad_.initialise(blend_fshdr)) {
        LOG_ERR("Failed to initialise quad");
        return false;
    }

    //quad2_.set_override(&fbuffer3_.get_attachment(0));

    blur_quad_.get_program()->bind();
    blur_quad_.get_program()->bind_uniform("blur_factor", 8.f/sc_width_);
    blur_quad_.get_program()->bind_uniform("direction", 0);
    blur_quad_.get_program()->bind_texture_unit("input_tex", 0);
    blur_quad_.get_program()->release();

    samp2_.allocate();
    samp2_.initialise();
    samp2_.set_min_filter(tex_filter::TF_LINEAR);
    samp2_.set_mag_filter(tex_filter::TF_LINEAR);
    samp2_.set_wrap_s(tex_wrap::TW_CLAMP_TO_EDGE);
    samp2_.set_wrap_t(tex_wrap::TW_CLAMP_TO_EDGE);
    samp2_.set_wrap_r(tex_wrap::TW_CLAMP_TO_EDGE);
    samp2_.release(0);

    skybox_ = make_mesh(generators::geometry3<vtx_p3_t, primitive_type::PT_TRIANGLES>::make_skybox<float>());
    if(!skybox_) {
        LOG_ERR("Failed to initialise skybox");
        return false;
    }

    if(!skybox_prog_.link(basic_vshdr, skybox_fshdr)) {
        LOG_ERR("Failed to build skybox fragment shader");
        return false;
    }

    skybox_ctx_.set_program(&skybox_prog_);
    skybox_ctx_.add_texture(gen_.prn_table(), gen_.grad1_table());
    skybox_ctx_.initialise();
    skybox_ctx_.set_texture_unit("perm", 0);
    skybox_ctx_.set_texture_unit("grad", 1);

    output_quad_.get_program()->bind();
    output_quad_.get_program()->bind_texture_unit("texA", 0);
    output_quad_.get_program()->bind_texture_unit("texB", 1);
    output_quad_.get_program()->release();

    stack.initialise();

    blend_state_.initialise(RS_BLEND | RS_DEPTH | RS_RASTERISATION);
    blend_state_.depth()->enabled = false;
    blend_state_.blend()->enabled = false;
    blend_state_.blend()->src_mode = render_state::blend_state::src_blend_mode::SBM_ONE_MINUS_DST_ALPHA;
    blend_state_.blend()->dst_mode = render_state::blend_state::dst_blend_mode::DBM_DST_ALPHA;

    stack.push_state(&blend_state_);

    depth_state_.initialise(RS_DEPTH | RS_BLEND);
    depth_state_.depth()->enabled = true;
    depth_state_.blend()->enabled = false;

    gl_error_check();

    return true;
}

void blur_glow::on_resize(int width, int height) {
    const int blur_radius = 6;
    cam_.frustum(45.f, width/float(height), .5f, 100.f);
    cam_.viewport(0, 0, width, height);
    cam_.frame(vec3f{0.f, 1.f, 0.f}, vec3f{0.f, 0.f, -1.f}, vec3f{0.f, 0.f, 2.f});
    blur_quad_.get_program()->bind();
    blur_quad_.get_program()->bind_uniform("blur_factor", float(blur_radius)/width);
    blur_quad_.get_program()->release();
    fbuffer1_.initialise(1, width/blur_radius, height/blur_radius, pixel_format::PF_RGBA, pixel_datatype::PD_UNSIGNED_BYTE, false, true);
    fbuffer2_.initialise(1, width, height, pixel_format::PF_RGBA, pixel_datatype::PD_UNSIGNED_BYTE, false, true);
    fbuffer3_.initialise(1, width, height, pixel_format::PF_RGBA, pixel_datatype::PD_UNSIGNED_BYTE, false, true);
    blur_quad_.resize(width, height);
    output_quad_.resize(width, height);
    gl_error_check();
}

void blur_glow::update(double t, float dt) {
    inc += .5f*dt;
}

void blur_glow::draw_scene() {
    stack.push_state(&depth_state_);
    const auto red_colour_table = {vec3f{0.f, 0.f, 0.f}, vec3f{1.f, 0.f, 0.f}, vec3f{1.f, 1.f, 0.f}, vec3f{1.f, 1.f, 1.f}};
    const auto blue_colour_table = {vec3f{0.f, 0.f, 0.f}, vec3f{0.f, 0.f, 1.f}, vec3f{0.f, 1.f, 1.f}, vec3f{1.f, 1.f, 1.f}};
    fbuffer1_.bind();
    rndr_.get_state_stack()->clear(0.f, 0.f, 0.f, 0.f);
    sphere_->bind();
    planet_ctx_.bind();

    // Low frequency tex
    planet_ctx_.set_parameter(pvm_idx, cam_.proj_view()
                                    * make_translation(-4.f, 0.f, -8.f)
                                    * make_rotation(vec3f{1.f, 0.f, 0.f}, PI<float>/2.f)
                                    * make_rotation(vec3f{0.f, 0.f, -1.f}, 4*inc));
    planet_ctx_.set_texture_unit(tex_idx, 0);
    planet_ctx_.set_parameter("colour[0]", red_colour_table);
    sphere_->draw(primitive_type::PT_TRIANGLES);

    // High frequency tex
    planet_ctx_.set_parameter(pvm_idx, cam_.proj_view()
                                    * make_translation(+0.f, 0.f, -4.f)
                                    * make_rotation(vec3f{1.f, 0.f, 0.f}, PI<float>/2.f)
                                    * make_rotation(vec3f{0.f, 0.f, 1.f}, inc));
    planet_ctx_.set_texture_unit(tex_idx, 1);
    planet_ctx_.set_parameter("colour[0]", blue_colour_table);
    sphere_->draw(primitive_type::PT_TRIANGLES);

    fbuffer1_.release();
    planet_ctx_.release();
    sphere_->release();
    stack.pop();

    // Horizontal Pass
    fbuffer2_.bind();
    rndr_.get_state_stack()->clear(0.f, 0.f, 0.f, 0.f);
    fbuffer1_.get_attachment(0).bind(0);
    samp2_.bind(0);
    blur_quad_.get_program()->bind();
    blur_quad_.get_program()->bind_uniform("direction", 0);
    blur_quad_.draw();
    samp2_.release(0);
    fbuffer1_.get_attachment(0).release();
    fbuffer2_.release();

    // Vertical Pass
    fbuffer3_.bind();
    rndr_.get_state_stack()->clear(0.f, 0.f, 0.f, 0.f);
    fbuffer2_.get_attachment(0).bind(0);
    samp2_.bind(0);
    blur_quad_.get_program()->bind();
    blur_quad_.get_program()->bind_uniform("direction", 1);
    blur_quad_.draw();
    samp2_.release(0);
    fbuffer2_.get_attachment(0).release();
    fbuffer3_.release();

    fbuffer2_.bind();
    rndr_.get_state_stack()->clear(0.f, 0.f, 0.f, 0.f);
    stack.push_state(&depth_state_);

    skybox_->bind();
    skybox_ctx_.set_parameter("PVM", cam_.proj_view()*make_scale(100.f, 100.f, 100.f));
    skybox_ctx_.bind();
    skybox_->draw(primitive_type::PT_TRIANGLES);
    skybox_ctx_.release();
    skybox_->release();

    sphere_->bind();
    planet_ctx_.bind();

    // Low frequency tex
    planet_ctx_.set_parameter(pvm_idx, cam_.proj_view()
                                    * make_translation(-4.f, 0.f, -8.f)
                                    * make_rotation(vec3f{1.f, 0.f, 0.f},  PI<float>/2.f)
                                    * make_rotation(vec3f{0.f, 0.f, -1.f}, 4*inc));
    planet_ctx_.set_texture_unit(tex_idx, 0);
    planet_ctx_.set_parameter("colour[0]", red_colour_table);
    sphere_->draw(primitive_type::PT_TRIANGLES);

    // High frequency tex
    planet_ctx_.set_parameter(pvm_idx, cam_.proj_view()
                                    * make_translation(+0.f, 0.f, -4.f)
                                    * make_rotation(vec3f{1.f, 0.f, 0.f}, PI<float>/2.f)
                                    * make_rotation(vec3f{0.f, 0.f, 1.f}, inc));
    planet_ctx_.set_texture_unit(tex_idx, 1);
    planet_ctx_.set_parameter("colour[0]", blue_colour_table);
    sphere_->draw(primitive_type::PT_TRIANGLES);

    planet_ctx_.release();
    sphere_->release();

    fbuffer2_.release();
    stack.pop();

    samp2_.bind(0);

    fbuffer3_.get_attachment(0).bind(0);
    fbuffer2_.get_attachment(0).bind(1);
    output_quad_.draw();
    fbuffer2_.get_attachment(0).release();
    fbuffer3_.get_attachment(0).release();

    samp2_.release(0);

    gl_error_check();
}

void blur_glow::draw() {
    draw_scene();
}

void blur_glow::shutdown() {
    fbuffer1_.deallocate();
    fbuffer2_.deallocate();
    fbuffer3_.deallocate();
    planet_tex1_.deallocate();
    planet_tex2_.deallocate();
    gen_.shutdown();
}

int main(int argc, char* argv[]) {
    blur_glow app;
    return app.run();
}

// Generators