#include <tools/log.hpp>
#include <maths/algebra.hpp>
#include <engine/program.hpp>
#include <engine/state_stack.hpp>
#include <host/GLFW/application.hpp>
#include <renderer/render_batch.hpp>
#include <graphics/graphics3/g3_types.hpp>
#include <maths/curves/catmull_rom.hpp>

using namespace zap;
using namespace maths;
using namespace engine;
using namespace graphics;
using namespace renderer;

const char* const vtx_shdr= GLSL(
    in vec2 position;
    in uint colour1;

    out vec4 colour;

    uniform mat4 mvp;

    vec4 decode(int colour) {
        return vec4((colour & 0x00FF0000) >> 16,
              (colour & 0x0000FF00) >> 8,
              (colour & 0x000000FF),
              (colour & 0xFF000000) >> 24)/255.;
    }

    void main() {
        colour = decode(int(colour1));
        gl_Position = mvp * vec4(position, 0, 1);
    }
);

const char* const frg_shdr = GLSL(
    in vec4 colour;
    out vec4 frag_colour;
    void main() {
        frag_colour = colour;
    }
);

class curve_app : public application {
public:
    using rndr_batch_t = render_batch<mesh_p2c1_t::vertex_stream_t>;
    curve_app() : application{"curve_app", 1366, 768} { }

    bool initialise() final;
    void update(double t, float dt) final;
    void draw() final;
    void shutdown() final;

    void on_resize(int width, int height) final;

    void generate_star(size_t steps, float inner=.55f, float outer=1.25f);

private:
    mat4f mvp_;
    program prog_;
    rndr_batch_t rndr_batch_;
    rndr_batch_t::token tok_;
    state_stack stack_;
    uint32_t counter_;
};

bool curve_app::initialise() {
    if(!rndr_batch_.initialise(10000)) {
        LOG("Failed to initialise");
        return false;
    }

    rndr_batch_.bind();
    tok_ = rndr_batch_.allocate(primitive_type::PT_LINES, 10000);
    rndr_batch_.release();

    if(!prog_.link(vtx_shdr, frg_shdr)) {
        LOG("Failed to initialise program");
        return false;
    } else {
        LOG("Program allocated");
    }

    auto proj = make_perspective(45.f, sc_width_/float(sc_height_), .5f, 10.f);
    auto look = look_at(vec3f{0.f, 0.f, -1.f}, vec3f{0.f, 0.f, 5.f});
    prog_.bind();
    prog_.bind_uniform("mvp", proj*look);

    return true;
}

void curve_app::update(double t, float dt) {
    static float timer = std::numeric_limits<float>::max();
    static size_t steps = 3;
    static float inner =.75f, outer = 1.25f;

    if(timer > 2.f) {
        generate_star(steps, inner, outer);
        steps = wrap<size_t>(steps+1, 2, 10);
        timer = 0.f;
    }

    timer += dt;
}

void curve_app::generate_star(size_t steps, float inner, float outer) {
    rndr_batch_.bind();
    rndr_batch_.map_write(tok_);

    size_t circular = 2*steps+1;
    counter_ = 0;

    std::vector<vec2f> vertices;
    const float inc = TWO_PI<float>/(2*steps);
    vertices.emplace_back(outer*vec2f{std::cos(0.f), std::sin(0.f)});
    for(int i = 1; i != circular; ++i) {
        if(i != circular-1) {
            vertices.emplace_back((i%2 == 0 ? outer : inner)*vec2f{std::cos(i*inc), std::sin(i*inc)});
            rndr_batch_.set(tok_, counter_++, vtx_p2c1_t{vertices[i-1], 0xFF0000FF});
            rndr_batch_.set(tok_, counter_++, vtx_p2c1_t{vertices[i], 0xFF0000FF});
        } else {
            rndr_batch_.set(tok_, counter_++, vtx_p2c1_t{vertices[i-1], 0xFF0000FF});
            rndr_batch_.set(tok_, counter_++, vtx_p2c1_t{vertices[0], 0xFF0000FF});
        }
    }
    curves::catmull_rom_spline<vec2f> curve{vertices, true};

    float t = 0.f;
    while(t < 1.f) {
        rndr_batch_.set(tok_, counter_++, vtx_p2c1_t{1.5f*curve.pos(t), 0xFFFFFFFF});
        t += .001f;
        rndr_batch_.set(tok_, counter_++, vtx_p2c1_t{1.5f*curve.pos(t), 0xFFFFFFFF});
    }

    rndr_batch_.unmap();
    rndr_batch_.release();
}

void curve_app::draw() {
    stack_.clear(0.f, 0.f, 0.f, 1.f);
    rndr_batch_.bind();
    rndr_batch_.draw(tok_, counter_);
    rndr_batch_.release();
}

void curve_app::shutdown() {
}

void curve_app::on_resize(int width, int height) {
    application::on_resize(width, height);
    auto proj = make_perspective(45.f, width/float(height), .5f, 10.f);
    auto look = look_at(vec3f{0.f, 0.f, -1.f}, vec3f{0.f, 0.f, 5.f});
    prog_.bind_uniform("mvp", proj*look);
}

int main(int argc, char* argv[]) {
    curve_app app{};
    app_config conf;
    conf.resizeable_window = true;
    return app.run(conf);
}
