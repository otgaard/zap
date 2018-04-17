//
// Created by Darren Otgaar on 2017/08/19.
//

#include <array>
#include <tools/log.hpp>
#include <maths/algebra.hpp>
#include <host/GLFW/application.hpp>
#include <engine/vertex_buffer.hpp>
#include <engine/index_buffer.hpp>
#include <engine/accessor.hpp>
#include <engine/program.hpp>
#include <graphics/graphics3/g3_types.hpp>
#include <graphics/loader/obj_loader.hpp>
#include <engine/state_stack.hpp>
#include <renderer/render_batch.hpp>
#include <maths/rand_functions.hpp>
#include <engine/pixmap.hpp>
#include <engine/texture.hpp>

using namespace zap;
using namespace zap::core;
using namespace zap::maths;
using namespace zap::engine;
using namespace zap::graphics;
using namespace zap::renderer;

// Provides examples of how to use buffers, accessors, and render batches in conjunction with models and procedurally
// generated geometry

inline float bias(float base, float x) { return std::pow(x, std::log(base) / std::log(.5f)); }
inline float arc_length(float radius, float theta) { return radius * theta; }
inline float step(float a, float x) { return (float)(x >= a); }
inline float pulse(float a, float b, float x) { return step(a, x) - step(b, x); }

pixmap<r8_t> make_sphere(size_t dim, float bias_factor) {
    pixmap<r8_t> img{ int(dim), int(dim), 1 };
    const float inv = 1.f / dim;
    const vec2f centre = { .5f, .5f }, inv_dims = { inv, inv };

    for (size_t r = 0; r != dim; ++r) {
        for (size_t c = 0; c != dim; ++c) {
            const vec2f V = (vec2f{ float(c), float(r) } *inv_dims) - centre;
            float len = 2.f * V.length();
            len = len > 1.f ? 1.f : len;
            img(c, r).set(lerp(bias(bias_factor, len), 255, 0));
        }
    }

    return img;
}

const char* const static_prog_vshdr = GLSL(
    uniform mat4 MVP;
    uniform mat4 MV;

    in vec3 position;
    in vec3 normal;
    in vec2 texcoord1;

    out vec2 tex;
    out vec3 nor;

    void main() {
        gl_Position = MVP * vec4(position, 1.);
        nor = (MV * vec4(normal, 0.)).xyz;
        tex = texcoord1;
    }
);

const char* const static_prog_fshdr = GLSL(
    uniform vec4 colour;
    uniform vec3 light_dir;

    in vec2 tex;
    in vec3 nor;

    out vec4 frag_colour;
    void main() {
        float s = max(dot(normalize(nor), light_dir), 0.);
        frag_colour = s * colour;
    }
);

const char* const stream_prog_vshdr = GLSL(
    uniform mat4 MVP;

    in vec3 position;
    in ivec4 colour1;

    out vec4 col;

    void main() {
        gl_Position = MVP * vec4(position, 1.);
        col = vec4(colour1 / 255.);
    }
);

const char* const stream_prog_fshdr = GLSL(
    in vec4 col;

    out vec4 frag_colour;
    void main() {
        frag_colour = col;
    }
);

const char* const particle_prog_vshdr = GLSL(
    uniform mat4 MVP;

    in vec3 position;
    in ivec4 colour1;
    in vec2 texcoord1;

    out vec4 col;
    out vec2 tex;

    void main() {
        gl_Position = MVP * vec4(position, 1.);
        col = vec4(colour1 / 255.);
        tex = texcoord1;
    }
);

const char* const particle_prog_fshdr = GLSL(
    uniform sampler2D diffuse_tex;
    in vec4 col;
    in vec2 tex;

    out vec4 frag_colour;
    void main() {
        frag_colour = col * texture(diffuse_tex, tex).rrrr;
    }
);

const size_t STATIC_VCOUNT = 200000;
const size_t STATIC_ICOUNT = 1000000;
const size_t STREAM_VCOUNT = 200000;
const size_t STREAM_ICOUNT = 1000000;

using p3c4_batch = render_batch<mesh_p3c4_t::vertex_stream_t>;
using p3n3t2_u32_batch = render_batch<mesh_p3n3t2_u32_t::vertex_stream_t, ibuf_u32_t>;
using p3c4t2_u32_batch = render_batch<mesh_p3c4t2_u32_t::vertex_stream_t, ibuf_u32_t>;

struct particles {
    static std::unique_ptr<particles> make_particles(size_t count) {
        auto ptr = std::make_unique<particles>();
        ptr->count = count;
        ptr->base_ptr = std::unique_ptr<byte[]>(new byte[count*particle_size]);
        byte* offset = ptr->base_ptr.get();
        ptr->position = reinterpret_cast<vec3f*>(offset); offset += sizeof(vec3f) * count;
        ptr->velocity = reinterpret_cast<vec3f*>(offset); offset += sizeof(vec3f) * count;
        ptr->rotation = reinterpret_cast<float*>(offset); offset += sizeof(float) * count;
        ptr->angular = reinterpret_cast<float*>(offset); offset += sizeof(float) * count;
        ptr->scale = reinterpret_cast<float*>(offset); offset += sizeof(float) * count;
        ptr->colourS = reinterpret_cast<vec4b*>(offset); offset += sizeof(vec4b) * count;
        ptr->colourD = reinterpret_cast<vec4b*>(offset); offset += sizeof(vec4b) * count;
        ptr->data = reinterpret_cast<vec4f*>(offset); offset += sizeof(vec4f) * count;
        assert(offset - ptr->base_ptr.get() == count * particle_size && "Invalid Size!");
        return ptr;
    }

    vec3f* position;
    vec3f* velocity;
    float* rotation;
    float* angular;
    float* scale;
    vec4b* colourS;
    vec4b* colourD;
    vec4f* data;

    std::unique_ptr<byte[]> base_ptr;
    size_t count;

    size_t size() const { return count; }

    static const size_t particle_size =
            sizeof(vec3f) +
            sizeof(vec3f) +
            sizeof(float) +
            sizeof(float) +
            sizeof(float) +
            sizeof(vec4b) +
            sizeof(vec4b) +
            sizeof(vec4f);
};

class dynamic_app : public application {
public:
    dynamic_app() : application{"dynamic_app", 1024, 768} { }

    bool initialise() final;
    void update(double t, float dt) final;
    void draw() final;
    void shutdown() final;

    void on_resize(int width, int height) final;

private:
    std::array<p3n3t2_u32_batch::token, 10> objects;
    p3n3t2_u32_batch static_batch_;
    mesh_p3c4t2_u32_t stream_mesh_;
    mesh_p3c4_t2_u32_t instanced_mesh_;
    program static_prog_;

    mat4f proj_matrix_;
    mat4f view_matrix_;

    state_stack rndr_state_;
    render_state default_state_{false, true, true, false};
    render_state additive_blend_state_{true, true, true, false};

    // Points or line segments
    p3c4_batch p3c4_batch_;
    std::vector<p3c4_batch::token> particle_batch_;
    program stream_prog_;
    p3c4_batch::token token_;

    // Camera-oriented quads
    p3c4t2_u32_batch quad_batch_;
    p3c4t2_u32_batch::token quads_;
    std::unique_ptr<particles> quad_particles_;
    program particle_prog_;
    texture quad_tex_;
};

bool dynamic_app::initialise() {
    stream_mesh_ = make_mesh<vtx_p3c4t2_t, uint32_t>(STREAM_VCOUNT, STREAM_ICOUNT);
    instanced_mesh_ = make_mesh<vtx_p3c4_t, vtx_t2_t, uint32_t>(STATIC_VCOUNT, 4, STATIC_ICOUNT);

    if(!static_batch_.initialise(STATIC_VCOUNT, STATIC_ICOUNT, buffer_usage::BU_STATIC_DRAW)) {
        LOG_ERR("Failed to allocate static batch");
        return false;
    }

    if(!stream_mesh_.is_allocated()) {
        LOG_ERR("Failed to allocate stream mesh");
        return false;
    }

#ifdef __APPLE__
    std::string models[3] = { "/Users/otgaard/Development/zap/assets/models/bunny.obj",
                              "/Users/otgaard/Development/zap/assets/models/dragon.obj",
                              "/Users/otgaard/Development/zap/assets/models/buddha.obj" };
#else
    std::string models[3] = { "D:/Development/zap/assets/models/bunny.obj",
                              "D:/Development/zap/assets/models/dragon.obj",
                              "D:/Development/zap/assets/models/buddha.obj" };
#endif

    int counter = 0;
    for (const auto &model : models) {
        auto obj = obj_loader::load_model(model);
        auto tok = static_batch_.allocate(primitive_type::PT_TRIANGLES, obj.first.size(), obj.second.size());
        static_batch_.load(tok, obj);
        objects[counter++] = tok;
    }

    // Test the Vertex Buffer only Render Batch
    if(!p3c4_batch_.initialise(100000, buffer_usage::BU_STREAM_DRAW)) {
        LOG_ERR("Failed to allocate particle_batch");
        return false;
    }

    token_ = p3c4_batch_.allocate(primitive_type::PT_LINES, 2000);

    auto rnd = rand_generator();

    if(token_.is_valid()) {
        particle_batch_.push_back(token_);
        const vec3f min = vec3f{-2.f, -2.f, -2.f}, max = vec3f{+2.f, +0.f, +2.f};

        if(p3c4_batch_.map_write(token_)) {
            for(int i = 0; i != 2000; ++i) {
                p3c4_batch_.set(token_, i, vtx_p3c4_t{ rnd.rand3f(min, max), rnd.rand4b() });
            }
            p3c4_batch_.unmap();
        }
    }

    // Test the Vertex Buffer + Index Buffer Render Batch
    if(!quad_batch_.initialise(4*1000, 6*1000, buffer_usage::BU_STREAM_DRAW)) {
        LOG_ERR("Failed to allocate quad_batch");
        return false;
    }

    {
        const int pcount = 500;
        quad_particles_ = particles::make_particles(pcount);

        auto& arr = *quad_particles_;
        const vec3f min = vec3f{-5.f, -5.f, -5.f}, max = vec3f{+5.f, +5.f, +5.f};
        for (int i = 0; i != pcount; ++i) {
            arr.position[i] = rnd.rand3f(min, max);
            arr.velocity[i] = rnd.rand3f({ -1.f, -1.f, -1.f }, { 1.f, 1.f, 1.f });
            arr.rotation[i] = rnd.rand1f(0.f, TWO_PI<float>);
            arr.angular[i] = rnd.rand1f();
            arr.scale[i] = rnd.rand1f(.05f, .1f);
            arr.colourS[i] = rnd.rand4b();
            arr.colourD[i] = rnd.rand4b();
            arr.data[i] = { 0.f, 0.f, 0.f, 0.f };
        }

        quads_ = quad_batch_.allocate(primitive_type::PT_TRIANGLES, 4*pcount, 6*pcount);
        if(quads_) {
            std::vector<vtx_p3c4t2_t> vertices(4*pcount);
            std::vector<uint32_t> indices(6*pcount);
            for(int i = 0; i != pcount; ++i) {
                const auto vidx = 4*i, iidx = 6*i;
                const auto P = arr.position[i];
                const auto C = arr.colourS[i];
                vertices[vidx+0] = vtx_p3c4t2_t{ P + arr.scale[i]*vec3f{-1.f, -1.f, 0.f}, C, vec2f{0.f, 0.f}};
                vertices[vidx+1] = vtx_p3c4t2_t{ P + arr.scale[i]*vec3f{+1.f, -1.f, 0.f}, C, vec2f{1.f, 0.f}};
                vertices[vidx+2] = vtx_p3c4t2_t{ P + arr.scale[i]*vec3f{+1.f, +1.f, 0.f}, C, vec2f{1.f, 1.f}};
                vertices[vidx+3] = vtx_p3c4t2_t{ P + arr.scale[i]*vec3f{-1.f, +1.f, 0.f}, C, vec2f{0.f, 1.f}};
                indices[iidx+0] = vidx+0; indices[iidx+1] = vidx+1; indices[iidx+2] = vidx+2;
                indices[iidx+3] = vidx+0; indices[iidx+4] = vidx+2; indices[iidx+5] = vidx+3;
            }

            quad_batch_.load(quads_, std::make_pair(vertices, indices));

            quad_tex_.set_mag_filter(tex_filter::TF_LINEAR);
            quad_tex_.set_min_filter(tex_filter::TF_LINEAR);
            quad_tex_.allocate();
            if(!quad_tex_.initialise(make_sphere(128, .3f))) {
                LOG_ERR("Failed to initialise texture");
                return false;
            }
        }

    }

    if(!static_prog_.link(static_prog_vshdr, static_prog_fshdr)) {
        LOG_ERR("Failed to link static_prog");
        return false;
    }

    if(!stream_prog_.link(stream_prog_vshdr, stream_prog_fshdr)) {
        LOG_ERR("Failed to link stream_prog");
        return false;
    }

    if(!particle_prog_.link(particle_prog_vshdr, particle_prog_fshdr)) {
        LOG_ERR("Failed to link particle_prog");
        return false;
    }

    if(!rndr_state_.initialise()) {
        LOG_ERR("Failed to initialise render state");
        return false;
    }

    default_state_.depth()->enabled = true;
    default_state_.rasterisation()->enable_culling = true;
    default_state_.rasterisation()->cull_face = rasterisation_state::cull_mode::CM_BACK;
    default_state_.rasterisation()->poly_mode = rasterisation_state::polygon_mode::PM_LINE;
    rndr_state_.push_state(&default_state_);

    additive_blend_state_.depth()->enabled = false;
    additive_blend_state_.blend()->enabled = true;
    additive_blend_state_.blend()->src_mode = blend_state::src_blend_mode::SBM_SRC_ALPHA;
    additive_blend_state_.blend()->dst_mode = blend_state::dst_blend_mode::DBM_ONE;

    return true;
}

void dynamic_app::update(double t, float dt) {
    static rand_generator rnd;
    static std::vector<vtx_p3c4_t> vertices(2000);
    static std::vector<vtx_p3c4t2_t> p_vertices(4*500);

    {
        const vec3f min = vec3f{-2.f, -2.f, -2.f}, max = vec3f{+2.f, +0.f, +2.f};
        for(int i = 0; i != 2000; ++i) vertices[i] = vtx_p3c4_t{rnd.rand3f(min, max), rnd.rand4b()};
        p3c4_batch_.load(token_, vertices);
    }

    {
        const uint32_t pcount = 500;
        const vec3f gravity = vec3f{0.f, -5.f, 0.f};
        const auto V = view_matrix_;

        auto& arr = *quad_particles_;
        for (size_t i = 0; i != pcount; ++i) {
            arr.position[i] += arr.velocity[i] * dt;
            arr.velocity[i] += gravity * dt;
            if (arr.position[i].length() > 10.f || arr.position[i].y < -1.f) {
                arr.position[i] = {0.f, 0.f, 0.f};
                arr.velocity[i] = 5.f * rnd.rand3f({-1.f, -1.f, -1.f}, {+1.f, +1.f, +1.f});
            }
        }

        for (uint32_t i = 0; i != pcount; ++i) {
            uint32_t vidx = 4 * i;

            const auto& P = arr.position[i];
            const auto& S = arr.scale[i];

            auto R = orthogonolise(normalise(P - vec3f{0.f, 2.f, 8.f}));

            p_vertices[vidx + 0] = vtx_p3c4t2_t{ P + R * S * vec3f{ -1.f, -1.f, 0.f }, arr.colourS[i], vec2f{ 0.f, 0.f } };
            p_vertices[vidx + 1] = vtx_p3c4t2_t{ P + R * S * vec3f{ +1.f, -1.f, 0.f }, arr.colourS[i], vec2f{ 1.f, 0.f } };
            p_vertices[vidx + 2] = vtx_p3c4t2_t{ P + R * S * vec3f{ +1.f, +1.f, 0.f }, arr.colourS[i], vec2f{ 1.f, 1.f } };
            p_vertices[vidx + 3] = vtx_p3c4t2_t{ P + R * S * vec3f{ -1.f, +1.f, 0.f }, arr.colourS[i], vec2f{ 0.f, 1.f } };
        }

        quad_batch_.load(quads_, p_vertices);
    }
}

void dynamic_app::draw() {
    static float angle = 0.f;
    angle = wrap(angle + .016f, 0.f, TWO_PI<float>);

    rndr_state_.clear(0.f, 0.f, 0.f, 1.f);

    static_prog_.bind();
    static_batch_.bind();
    for(int i = 0; i != 3; ++i) {
        auto MV = view_matrix_ * make_translation(-2.f + 2.f*i, 0.f, 0.f) * make_scale(.2f, .2f, .2f) * make_rotation(vec3f{0.f, 1.f, 0.f}, angle);
        static_prog_.bind_uniform("MVP", proj_matrix_ * MV);
        static_prog_.bind_uniform("MV", MV);
        static_batch_.draw(objects[i]);
    }
    static_batch_.release();
    static_prog_.release();

    stream_prog_.bind();
    p3c4_batch_.bind();
    stream_prog_.bind_uniform("MVP", proj_matrix_ * view_matrix_ * make_rotation(vec3f{0.f, 1.f, 0.f}, angle));
    p3c4_batch_.draw(token_);
    p3c4_batch_.release();
    stream_prog_.release();

    rndr_state_.push_state(&additive_blend_state_);
    particle_prog_.bind();
    quad_batch_.bind();
    particle_prog_.bind_uniform("MVP", proj_matrix_ * view_matrix_);
    quad_tex_.bind(0);
    quad_batch_.draw(quads_);
    quad_tex_.release();
    quad_batch_.release();
    particle_prog_.release();
    rndr_state_.pop();
}

void dynamic_app::shutdown() {
}

void dynamic_app::on_resize(int width, int height) {
    proj_matrix_ = make_perspective(40.f, float(width)/height, .5f, 100.f);
    view_matrix_ = look_at(vec3f{0.f, 0.f, 0.f}, vec3f{0.f, 2.f, 8.f});

    static_prog_.bind();
    static_prog_.bind_uniform("MVP", proj_matrix_ * view_matrix_ * make_scale(.2f, .2f, .2f));
    static_prog_.bind_uniform("MV", view_matrix_);
    static_prog_.bind_uniform("light_dir", normalise(vec3f{1.f, 1.f, 1.f}));
    static_prog_.bind_uniform("colour", vec4f{1.f, 1.f, 1.f, 1.f});
    static_prog_.release();

    stream_prog_.bind();
    stream_prog_.bind_uniform("MVP", proj_matrix_ * view_matrix_);
    stream_prog_.release();

    particle_prog_.bind();
    particle_prog_.bind_uniform("MVP", proj_matrix_ * view_matrix_);
    particle_prog_.bind_uniform("diffuse_tex", 0);
    particle_prog_.release();
}

int main(int argc, char* argv[]) {
    dynamic_app app{};
    return app.run();
}
