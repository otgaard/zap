/* Created by Darren Otgaar on 2016/06/26. http://www.github.com/otgaard/zap */
#include "plotter2.hpp"
#include "apps/graphic_types.hpp"
#include <generators/noise/perlin.hpp>

using vbuf_p2_t = vertex_buffer<vtx_p2_t, buffer_usage::BU_DYNAMIC_DRAW>;
using mesh_p2_t = mesh<vertex_stream<vbuf_p2_t>, primitive_type::PT_LINE_STRIP>;

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
        frag_colour = vec4(1,1,1,1);
    }
);

struct plotter2::state_t {
    vbuf_p2_t buffer;
    mesh_p2_t plot_mesh;
    program style;
    generators::perlin<float> noise;
};

plotter2::plotter2() : state_(nullptr) {
    state_ = std::make_unique<state_t>();
}

plotter2::~plotter2() {
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


    s.style.bind();

    /*
    s.uniform.allocate();
    s.uniform.bind();
    gl_error_check();
    s.uniform.initialise(nullptr);
    if(s.uniform.map(buffer_access::BA_WRITE_ONLY)) {
        auto& ref = s.uniform.ref();
        ref.cam_projection = make_perspective<float>(45.f, 1280.f/768.f, 1.f, 100.f);
        ref.mv_matrix = make_translation<float>(0, 0, -4.f);
        s.uniform.unmap();
    }

    auto loc = s.style.uniform_block_index("transform");
    s.uniform.bind_point(loc);
    */

    //auto proj_mat = make_perspective(45.f, 1280.f/768.f, 1.f, 100.f);
    mat4f proj_mat = {
            2.f, 0.f, 0.f, -1.f,
            0.f, 2.f, 0.f, -1.f,
            0.f, 0.f, 2.f, -1.f,
            0.f, 0.f, 0.f, 1.f
    };

    //proj_mat = make_perspective<float>(45.f, 1280.f/768.f, 0.5f, 10.f);
    auto mv_mat = make_translation<float>(0.5f, 0.5f, 0.f) * make_scale<float>(0.45f, 0.45f, 0.f);
    s.style.bind_uniform("proj_matrix", proj_mat);
    s.style.bind_uniform("mv_matrix", mv_mat);
    gl_error_check();

    if(!s.plot_mesh.allocate() || !s.buffer.allocate()) {
        LOG_ERR("Error allocating plotter2 mesh or vertex buffer");
        return false;
    }

    s.plot_mesh.set_stream(&s.buffer);
    s.plot_mesh.bind();
    s.buffer.bind();
    s.buffer.initialise(1005);

    if(s.buffer.map(buffer_access::BA_WRITE_ONLY)) {
        const float start = -1.f;
        const float scale = 2.f/(1000-1);
        for(size_t i = 0; i != 1000; ++i) s.buffer[i].position.set(start + i*scale, 0);

        s.buffer[1000].position.set(-1,-1);
        s.buffer[1001].position.set(+1,-1);
        s.buffer[1002].position.set(+1,+1);
        s.buffer[1003].position.set(-1,+1);
        s.buffer[1004].position.set(-1,-1);
        s.buffer.unmap();
    }

    s.plot_mesh.release();
    return true;
}

static float angle = 0.f;

void plotter2::draw() {
    if(!state_) return;
    auto& s = *state_;

    s.buffer.bind();
    if(s.buffer.map(buffer_access::BA_WRITE_ONLY)) {
        for(size_t i = 0; i != 1000; ++i) s.buffer[i].position.y = 6*s.noise.turbulence(angle + 0.001f*i, 4, .5f, 2.f) - 1.f;
        s.buffer.unmap();
    }
    s.buffer.release();

    angle += 0.005f;

    s.style.bind();
    s.plot_mesh.bind();
    s.plot_mesh.draw(primitive_type::PT_LINE_STRIP, 0, 1000);
    s.plot_mesh.draw(primitive_type::PT_LINE_STRIP, 1000, 5);
    s.plot_mesh.release();
    s.style.release();
}
