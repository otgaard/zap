/* Created by Darren Otgaar on 2016/03/05. http://www.github.com/otgaard/zap */
#include <map>
#include <maths/io.hpp>
#include <tools/log.hpp>
#include <host/GLFW/application.hpp>
#include <engine/framebuffer.hpp>
#include <engine/mesh.hpp>
#include <graphics3/g3_types.hpp>
#include <generators/geometry/geometry3.hpp>
#include <renderer/render_context.hpp>
#include <renderer/shader_builder.hpp>
#include <engine/state_stack.hpp>
#include <graphics2/text/text_batcher.hpp>
#include <graphics2/quad.hpp>
#include <graphics2/text/text.hpp>

using namespace zap;
using namespace zap::maths;
using namespace zap::engine;
using namespace zap::renderer;
using namespace zap::graphics;

using p3n3t2_gen = generators::geometry3<graphics::vtx_p3n3t2_t, primitive_type::PT_TRIANGLES>;

// Some tests
const char* const quote1 = "\"Would you tell me, please, which way I ought to go from here?\"\n"
        "\"That depends a good deal on where you want to get to.\"\n"
        "\"I don't much care where...\"\n"
        "\"Then it doesn't matter which way you go.\"";

const char* const quote2 = "    But the Raven, sitting lonely on the placid bust, spoke only\n"
        "That one word, as if his soul in that one word he did outpour.\n"
        "    Nothing farther then he uttered - not a feather then he fluttered -\n"
        "    Till I scarcely more than muttered \"Other friends have flown before -\n"
        "On the morrow he will leave me, as my Hopes have flown before.\"\n"
        "            Then the bird said \"Nevermore.\"";

#if defined(_WIN32)
const char* const arial_font = "C:\\Windows\\Fonts\\arial.ttf";
#elif defined(__APPLE__)
const char* const arial_font = "/Library/Fonts/Arial.ttf";
#endif

class zap_example : public application {
public:
    zap_example() : application("zap_example", 1280, 768) { }

    bool initialise() final;
    void update(double t, float dt) final;
    void draw() final;
    void shutdown() final;

    void on_resize(int width, int height) final;
    void on_mousemove(double x, double y) final;
    void on_mousewheel(double xinc, double yinc) final;

protected:
    framebuffer shadow_map_;
    std::unique_ptr<mesh_base> sphere_;
    std::unique_ptr<render_context> context_;
    state_stack rndr_state_;

    text_batcher batcher;
    camera cam_;
};

bool zap_example::initialise() {
    if(!batcher.initialise()) {
        LOG_ERR("Failed to initialise batcher");
        return false;
    }

    auto arial = batcher.add_font(arial_font, 20);
    if(arial) {
        LOG(arial->name, arial->px_height, arial->font_id);
    }

    auto tex = batcher.get_texture(arial->font_id);
    LOG(arial->name, tex->is_allocated(), tex->width(), tex->height());

    UNUSED(quote1); UNUSED(quote2);

    // Test building a text object
    auto quote1_proxy = batcher.create_text(arial->font_id, quote1);
    LOG("Proxy:", quote1_proxy.get_font()->name, quote1_proxy.get_text(), quote1_proxy.get_id());

    auto quote2_proxy = batcher.create_text(arial->font_id, quote2);
    LOG("Proxy:", quote2_proxy.get_font()->name, quote2_proxy.get_text(), quote2_proxy.get_id());
    quote2_proxy.translate(vec2i{100, 200});

    auto quote3_proxy = batcher.create_text(arial->font_id, quote2);
    LOG("Proxy:", quote3_proxy.get_font()->name, quote3_proxy.get_text(), quote3_proxy.get_id());
    quote3_proxy.translate(vec2i{200, 400});


    sphere_ = p3n3t2_gen::make_mesh(p3n3t2_gen::make_UVsphere(10, 30, 1.f, false));
    if(!sphere_->is_allocated()) {
        LOG_ERR("Failed to allocate sphere mesh");
        return false;
    }

    if(!rndr_state_.initialise()) {
        LOG_ERR("Failed to initialise render state");
        return false;
    }

    builder_task<> task;
    task.method = builder_task<>::lighting_method::LM_NONE;
    task.diffuse_map = builder_task<>::texture_type::TT_NONE;
    context_ = shader_builder::build_basic_lights(task);

    return true;
}

void zap_example::on_resize(int width, int height) {
    auto proj_matrix = make_perspective(45.f, width/float(height), 1.f, 10.f);
    auto mv_matrix = make_translation(0.f, 0.f, -5.f);
    context_->set_parameter("mvp_matrix", proj_matrix * mv_matrix);
    context_->set_parameter("colour", vec4f{1.f, 1.f, 0.f, 1.f});

    cam_.set_perspective(false);
    cam_.frustum(0, width, height, 0, 0, 100);
    cam_.orthogonolise(vec3f{0.f, 0.f, -1.f});
}

void zap_example::on_mousemove(double x, double y) {
}

void zap_example::on_mousewheel(double xinc, double yinc) {
}

void zap_example::update(double t, float dt) {
}

void zap_example::draw() {
    rndr_state_.clear();
    context_->bind();
    sphere_->bind();
    sphere_->draw();
    sphere_->release();
    context_->release();

    batcher.draw(cam_);
}

void zap_example::shutdown() {
}

int main(int argc, char* argv[]) {
    zap_example app{};
    app_config config;
    config.resizeable_window = true;
    return app.run(config);
}
