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
    in int colour1;

    out vec4 colour;

    uniform mat4 mvp;

    vec4 decode(int colour) {
        return vec4((colour1 & 0x00FF0000) >> 16,
              (colour1 & 0x0000FF00) >> 8,
              (colour1 & 0x000000FF),
              (colour1 & 0xFF000000) >> 24)/255.;
    }

    void main() {
        colour = decode(colour1);
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

private:
    mat4f mvp_;
    program prog_;
    rndr_batch_t rndr_batch_;
    rndr_batch_t::token tok_;
    state_stack stack_;
};

bool curve_app::initialise() {
    if(!rndr_batch_.initialise(10000)) {
        LOG("Failed to initialise");
        return false;
    }

    rndr_batch_.bind();
    tok_ = rndr_batch_.allocate(primitive_type::PT_LINES, 10000);
    rndr_batch_.map_write(tok_);

    std::vector<vec2f> vertices;
    const float inc = TWO_PI<float>/40;
    vertices.emplace_back(vec2f{std::cos(-2.f), std::sin(-2.f)});
    for(int i = 1; i != 40; ++i) {
        vertices.emplace_back((i%2==0 ? 1.25f : .75f)*vec2f{std::cos(i*inc-2.f), std::sin(i*inc-2.f)});
        rndr_batch_.set(tok_, 2*i, vtx_p2c1_t{vertices[i-1]-vec2f{0.f, .5f}, 0x00FF0000});
        rndr_batch_.set(tok_, 2*i+1, vtx_p2c1_t{vertices[i]-vec2f{0.f, .5f}, 0x00FF0000});
    }
    curves::catmull_rom_spline<vec2f> curve{vertices, true};

    float t = 0.f;
    int counter = 80;
    while(t < 1.f) {
        rndr_batch_.set(tok_, counter++, vtx_p2c1_t{curve.pos(t), 0xFFFF00FF});
        t += .001f;
        if(t > 1.f) {
            rndr_batch_.set(tok_, counter++, vtx_p2c1_t{curve.pos(1.f), 0xFFFF00FF});
            break;
        }
        rndr_batch_.set(tok_, counter++, vtx_p2c1_t{curve.pos(t), 0xFFFF00FF});
    }

    rndr_batch_.unmap();
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
}

void curve_app::draw() {
    stack_.clear(0.f, 0.f, 0.f, 1.f);
    rndr_batch_.bind();
    rndr_batch_.draw(tok_);
    rndr_batch_.release();
}

void curve_app::shutdown() {
}

void curve_app::on_resize(int width, int height) {
}

int main(int argc, char* argv[]) {
    curve_app app{};
    return app.run();
}
