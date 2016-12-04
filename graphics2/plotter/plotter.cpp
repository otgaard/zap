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

    size_t grid_offset;             // The offset of the buffer position pointing to the end of the grid lines
    size_t vtx_offset;              // The "write" location for static graphs
    std::vector<size_t> lines;
};

plotter::plotter() : reserve_(10000), live_(false), live_samples_(0), state_(new state_t()), s(*state_.get()) {
    s.grid_offset = 0;
}

plotter::plotter(const vec2f& domain, const vec2f& range, const float majors) : reserve_(10000), live_(false), live_samples_(0),
                                                                                state_(new state_t()), s(*state_.get()) {
    s.grid.domain = domain;
    s.grid.range = range;
    s.grid.domain_maj_min.x = majors;
    s.grid.range_maj_min.x = majors;
    s.grid_offset = 0;
}

plotter::~plotter() = default;

bool plotter::is_initialised() const {
    return s.mesh.is_allocated();
}

size_t plotter::capacity() const {
    return s.vbuf.vertex_count() - s.vtx_offset;
}

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

    // Draw the live plot
    if(live_) s.mesh.draw(primitive_type::PT_LINE_STRIP, s.vtx_offset, live_samples_);

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

    s.vbuf.initialise(vertex_count+reserve_);

    auto x_inc = (s.grid.domain.y - s.grid.domain.x)/(x_samples-1);
    auto y_inc = (s.grid.range.y - s.grid.range.x)/(y_samples-1);

    if(s.vbuf.map(buffer_access::BA_WRITE_ONLY)) {
        for(int i = 0; i != x_samples; ++i) {
            auto idx = 2*i;
            auto x_off = s.grid.domain.x + i*x_inc;

            s.vbuf[idx  ].position = vec2f(x_off, s.grid.range.x);
            s.vbuf[idx+1].position = vec2f(x_off, s.grid.range.y);
            for(int c = 0; c != 2; ++c) s.vbuf[idx+c].colour1.set(0, 0, 0);
        }

        size_t offset = 2*x_samples;
        for(int i = 0; i != y_samples; ++i) {
            auto idx = offset + 2*i;
            auto y_off = s.grid.range.x + i*y_inc;

            s.vbuf[idx  ].position = vec2f(s.grid.domain.x, y_off);
            s.vbuf[idx+1].position = vec2f(s.grid.domain.y, y_off);
            for(int c = 0; c != 2; ++c) s.vbuf[idx+c].colour1.set(0, 0, 0);
        }

        s.vbuf.unmap();
        success = true;
    }

    s.grid_offset = vertex_count;
    s.vtx_offset = vertex_count;
    s.mesh.release();
    return success && !gl_error_check();
}

bool plotter::map_buffer() {
    s.vbuf.bind();
    return s.vbuf.map(buffer_access::BA_READ_WRITE) != nullptr;
}

void plotter::unmap_buffer() {
    s.vbuf.unmap();
    s.vbuf.release();
}

void plotter::set_position(size_t idx, const vec2f& P) {
    if(s.vbuf.is_mapped()) {
        s.vbuf[idx].position = P;
    }
}

void plotter::set_colour(size_t idx, const vec3b& C) {
    if(s.vbuf.is_mapped()) {
        s.vbuf[idx].colour1 = C;
    }
}

size_t plotter::vertex_count() const {
    return s.vtx_offset;
}

void plotter::add_plot_vertices(size_t samples) {
    s.vtx_offset += samples;
    s.lines.push_back(s.vtx_offset);
}
