/* Created by Darren Otgaar on 2016/11/27. http://www.github.com/otgaard/zap */
#include <engine/program.hpp>
#include <renderer/camera.hpp>

#include "plotter.hpp"
#include "graphics2/graphics2_types.hpp"

using namespace zap;
using namespace graphics;

const char* const line_vtx_shdr = GLSL(
    uniform mat4 PVM;
    in vec2 position;
    in vec3 colour1;
    out vec3 colour;

    void main() {
        colour = colour1;
        gl_Position = PVM * vec4(position, 0, 1);
    }
);

const char* const line_frg_shdr = GLSL(
    in vec3 colour;
    out vec4 frag_colour;

    void main() {
        frag_colour = vec4(colour, 1.0);
    }
);

struct plotter::state_t {
    plotter::grid grid_;

    vbuf_plot_t vbuf_;
    mesh_plot_ls_t mesh_;
    program prog_;
};

plotter::plotter() : state_(new state_t()), s(*state_.get()) {
}

plotter::plotter(const vec2f& domain, const vec2f& range, const float majors) : state_(new state_t()), s(*state_.get()) {
    s.grid_.domain = domain;
    s.grid_.range = range;
    s.grid_.domain_maj_min.x = majors;
    s.grid_.range_maj_min.x = majors;
}

plotter::~plotter() = default;

void plotter::set_grid(const grid& g) {
    s.grid_ = g;
}

bool plotter::initialise() {
    if(!s.vbuf_.allocate() || !s.mesh_.allocate()) {
        LOG_ERR("Failure to initialise plotter resources");
        return false;
    }

    s.prog_.add_shader(shader_type::ST_VERTEX, line_vtx_shdr);
    s.prog_.add_shader(shader_type::ST_FRAGMENT, line_frg_shdr);
    if(!s.prog_.link()) {
        LOG_ERR("Error linking plotter program");
        return false;
    }

    if(!build_grid()) {
        LOG_ERR("Failed to build plotter grid");
        return false;
    }

    return true;
}

void plotter::update(double t, float dt) {
}

void plotter::draw(const renderer::camera& cam) {
    s.prog_.bind();
    s.prog_.bind_uniform("PVM", cam.proj_view()*make_translation(1280/2.f, 768.f/2, 0.f));
    s.mesh_.bind();
    s.mesh_.draw();
    s.mesh_.release();
    s.prog_.release();
}

bool plotter::build_grid() {
    s.mesh_.bind(); s.vbuf_.bind();
    s.mesh_.set_stream(&s.vbuf_);

    const size_t samples = 100;
    const size_t vertex_count = 4*samples;  // 2 per line, 2 axes

    // We'll do 100 samples for now
    s.vbuf_.initialise(vertex_count);

    auto x_inc = (s.grid_.domain.y - s.grid_.domain.x)/(samples-1);
    auto y_inc = (s.grid_.range.y - s.grid_.range.x)/(samples-1);
    LOG(x_inc, y_inc);

    if(s.vbuf_.map(buffer_access::BA_WRITE_ONLY)) {
        for(int i = 0; i != samples; ++i) {
            auto idx = 4*i;
            auto x_off = s.grid_.domain.x + i*x_inc, y_off = s.grid_.range.x + i*y_inc;

            s.vbuf_[idx  ].position = vec2f(x_off, s.grid_.range.x);
            s.vbuf_[idx+1].position = vec2f(x_off, s.grid_.range.y);
            s.vbuf_[idx+2].position = vec2f(s.grid_.domain.x, y_off);
            s.vbuf_[idx+3].position = vec2f(s.grid_.domain.y, y_off);
        }
        s.vbuf_.unmap();
    }
    s.mesh_.release();

    return true;
}
