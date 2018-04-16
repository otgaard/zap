/* Created by Darren Otgaar on 2016/06/26. http://www.github.com/otgaard/zap */
#include <graphics/graphics2/plotter2.hpp>
#include <engine/engine.hpp>
#include <engine/vertex_buffer.hpp>
#include <engine/mesh.hpp>
#include <engine/program.hpp>

using namespace zap;
using namespace zap::maths;
using namespace zap::engine;

#define GLSL(src) "#version 330 core\n" #src

namespace zap { namespace graphics2 {

using pos2f_t = core::position<vec2f>;
using vtx_p2_t = vertex<pos2f_t>;
using vbuf_p2_t = vertex_buffer<vtx_p2_t>;
using mesh_p2_t = mesh<vertex_stream<vbuf_p2_t>>;

const char* plotter2_vtx_src = GLSL(
    uniform mat4 proj_matrix;
    uniform mat4 mv_matrix;

    in vec2 position;

    void main() {
        gl_Position = proj_matrix * mv_matrix * vec4(position.xy, 0, 1);
    }
);

const char* plotter2_frg_src = GLSL(
    out vec4 frag_colour;

    void main() {
        frag_colour = vec4(1, 1, 1, 1);
    }
);

struct plotter2::state_t {
    vbuf_p2_t buffer;
    mesh_p2_t plot_mesh;
    program style;
};

plotter2::plotter2() : state_(new state_t) {
}

plotter2::~plotter2() {
}

void plotter2::push_value(float value) {
    auto& s = *state_;
    s.buffer.bind();
    if(s.buffer.map(buffer_access::BA_READ_WRITE)) {
        shift_array<vtx_p2_t, 1000>(s.buffer.data(), 1, [](vtx_p2_t& a, const vtx_p2_t& b) {
            a.position.y = b.position.y;
        });
        //for(size_t i = 999; i != 0; --i) s.buffer[i].position.y = s.buffer[i-1].position.y;
        s.buffer[0].position.y = value;
        s.buffer.unmap();
    }
    s.buffer.release();
}

void plotter2::set_data(const std::array<float,1000>& data) {
    auto& s = *state_;
    s.buffer.bind();
    if(s.buffer.map(buffer_access::BA_WRITE_ONLY)) {
        for(size_t i = 0; i != 1000; ++i) s.buffer[i].position.y = clamp(data[i]);
        s.buffer.unmap();
    }
    s.buffer.release();
}

void plotter2::set_data(const std::array<vec2f,1000>& data) {
    auto& s = *state_;
    s.buffer.bind();
    if(s.buffer.map(buffer_access::BA_WRITE_ONLY)) {
        for(size_t i = 0; i != 1000; ++i) s.buffer[i].position = data[i];
        s.buffer.unmap();
    }
    s.buffer.release();
}

void plotter2::transform(const mat4f& transform) {
    state_->style.bind();
    state_->style.bind_uniform("mv_matrix", transform);
    state_->style.release();
}

bool plotter2::initialise() {
    if(!state_) return false;
    auto& s = *state_;

    s.style.add_shader(new shader(shader_type::ST_VERTEX, plotter2_vtx_src));
    s.style.add_shader(new shader(shader_type::ST_FRAGMENT, plotter2_frg_src));
    if(!s.style.link(true)) {
        LOG_ERR("Couldn't link style for plotter.");
        return false;
    }

    if(!s.plot_mesh.allocate() || !s.buffer.allocate()) {
        LOG_ERR("Error allocating plotter2 mesh or vertex buffer");
        return false;
    }

    s.plot_mesh.set_stream(&s.buffer);
    s.plot_mesh.bind();
    s.buffer.bind();
    s.buffer.initialise(1004);

    if(s.buffer.map(buffer_access::BA_WRITE_ONLY)) {
        const float start = -1.f;
        const float scale = 2.f / (1000 - 1);

        for(size_t i = 0; i != 1000; ++i) s.buffer[i].position.set(start + i*scale, 0);

        s.buffer[1000].position.set(-1, -1);
        s.buffer[1001].position.set(+1, -1);
        s.buffer[1002].position.set(+1, +1);
        s.buffer[1003].position.set(-1, +1);

        s.buffer.unmap();
    }

    s.plot_mesh.release();
    return true;
}

void plotter2::draw() {
    if(!state_) return;
    auto& s = *state_;

    s.style.bind();
    s.plot_mesh.bind();
    s.plot_mesh.draw(primitive_type::PT_LINE_STRIP, 0, 1000);
    s.plot_mesh.draw(primitive_type::PT_LINE_LOOP, 1000, 4);
    s.plot_mesh.release();
    s.style.release();
}

void plotter2::set_projection_matrix(const mat4f& proj_mat) {
    state_->style.bind();
    state_->style.bind_uniform("proj_matrix", proj_mat);
    state_->style.release();
}

void plotter2::set_mv_matrix(const mat4f& mv_mat) {
    state_->style.bind();
    state_->style.bind_uniform("mv_matrix", mv_mat);
    state_->style.release();
}

}}