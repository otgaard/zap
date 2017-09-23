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

using namespace zap;
using namespace zap::maths;
using namespace zap::engine;
using namespace zap::renderer;
using namespace zap::graphics;

using p3n3t2_gen = generators::geometry3<graphics::vtx_p3n3t2_t, primitive_type::PT_TRIANGLES>;

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
    quad test_quad;
};

bool zap_example::initialise() {
    if(!batcher.initialise()) {
        LOG_ERR("Failed to initialise batcher");
        return false;
    }

    auto arial = batcher.add_font("/Library/Fonts/Arial.ttf", 20);
    if(arial) {
        LOG(arial->name, arial->pixel_height, arial->font_id);
    }

    if(!test_quad.initialise()) {
        LOG_ERR("Error initialising test_quad");
        return false;
    }

    auto tex = batcher.get_texture(arial->font_id);
    LOG(arial->name, tex->is_allocated(), tex->width(), tex->height());
    test_quad.set_override(tex);

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

    test_quad.resize(width, height);
}

void zap_example::on_mousemove(double x, double y) {
}

void zap_example::on_mousewheel(double xinc, double yinc) {
}

void zap_example::update(double t, float dt) {

}

void zap_example::draw() {
    test_quad.draw();

    context_->bind();
    sphere_->bind();
    sphere_->draw();
    sphere_->release();
    context_->release();
}

void zap_example::shutdown() {
}

int main(int argc, char* argv[]) {
    zap_example app{};
    return app.run();
}
