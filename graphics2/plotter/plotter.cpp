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
    in uvec4 colour1;
    out vec3 colour;

    const float inv_byte = 1.0/255.0;

    void main() {
        colour = vec3(colour1.r*inv_byte, colour1.g*inv_byte, colour1.b*inv_byte);
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
    plotter::grid grid;

    vbuf_plot_t vbuf;
    mesh_plot_ls_t mesh;
    program prog;

    size_t grid_offset;
    size_t vtx_offset;
    std::vector<size_t> lines;
};

plotter::plotter() : state_(new state_t()), s(*state_.get()) {
}

plotter::plotter(const vec2f& domain, const vec2f& range, const float majors) : state_(new state_t()), s(*state_.get()) {
    s.grid.domain = domain;
    s.grid.range = range;
    s.grid.domain_maj_min.x = majors;
    s.grid.range_maj_min.x = majors;
    s.grid_offset = 0;
}

plotter::~plotter() = default;

void plotter::set_grid(const grid& g) {
    s.grid = g;
}

bool plotter::initialise() {
    if(!s.vbuf.allocate() || !s.mesh.allocate()) {
        LOG_ERR("Failure to initialise plotter resources");
        return false;
    }

    s.prog.add_shader(shader_type::ST_VERTEX, line_vtx_shdr);
    s.prog.add_shader(shader_type::ST_FRAGMENT, line_frg_shdr);
    if(!s.prog.link()) {
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
    s.prog.bind();
    s.prog.bind_uniform("PVM", cam.proj_view() * world_transform.gl_matrix());

    s.mesh.bind();
    s.mesh.draw(primitive_type::PT_LINES, 0, s.grid_offset);

    size_t offset = s.grid_offset;
    for(int i = 0; i != s.lines.size(); ++i) {
        s.mesh.draw(primitive_type::PT_LINE_STRIP, offset, s.lines[i] - offset);
        offset = s.lines[i];
    }

    s.mesh.release();
    s.prog.release();
}

bool plotter::build_grid() {
    bool success = false;
    s.mesh.bind(); s.vbuf.bind();
    s.mesh.set_stream(&s.vbuf);

    const size_t x_samples = (size_t)(std::round(s.grid.domain.y - s.grid.domain.x)/s.grid.domain_maj_min.x + 1);
    const size_t y_samples = (size_t)(std::round(s.grid.range.y - s.grid.range.x)/s.grid.range_maj_min.x + 1);
    const size_t vertex_count = 2*x_samples + 2*y_samples;

    s.vbuf.initialise(vertex_count+10000);  // Reserve a fixed 10000 for now

    auto x_inc = (s.grid.domain.y - s.grid.domain.x)/(x_samples-1);
    auto y_inc = (s.grid.range.y - s.grid.range.x)/(y_samples-1);

    if(s.vbuf.map(buffer_access::BA_WRITE_ONLY)) {
        for(int i = 0; i != x_samples; ++i) {
            auto idx = 2*i;
            auto x_off = s.grid.domain.x + i*x_inc;

            s.vbuf[idx  ].position = vec2f(x_off, s.grid.range.x);
            s.vbuf[idx+1].position = vec2f(x_off, s.grid.range.y);
            for(int c = 0; c != 2; ++c) s.vbuf[idx+c].colour1.set(50, 50, 50);
        }

        size_t offset = 2*x_samples;
        for(int i = 0; i != y_samples; ++i) {
            auto idx = offset + 2*i;
            auto y_off = s.grid.range.x + i*y_inc;

            s.vbuf[idx  ].position = vec2f(s.grid.domain.x, y_off);
            s.vbuf[idx+1].position = vec2f(s.grid.domain.y, y_off);
            for(int c = 0; c != 2; ++c) s.vbuf[idx+c].colour1.set(50, 50, 50);
        }

        s.vbuf.unmap();
        success = !gl_error_check();
    }

    s.grid_offset = vertex_count;
    s.vtx_offset = vertex_count;
    s.mesh.release();
    return success;
}

void plotter::add_plot(const sampler1D<float>& obj, size_t samples, const vec3b& colour) {
    const float inv_x = 1.f/(samples-1);

    s.vbuf.bind();
    if(s.vbuf.map(buffer_access::BA_WRITE_ONLY)) {
        for(int i = 0; i != samples; ++i) {
            auto idx = s.vtx_offset + i;

            s.vbuf[idx].position.set(inv_x * i, obj(inv_x * i));
            s.vbuf[idx].colour1 = colour;
        }
        s.vbuf.unmap();
    }
    s.vbuf.release();

    s.lines.push_back(s.vtx_offset + samples);
    s.vtx_offset += samples;
}
