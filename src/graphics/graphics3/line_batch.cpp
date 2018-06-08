/* Created by Darren Otgaar on 2018/04/22. http://www.github.com/otgaard/zap */
#include "line_batch.hpp"

namespace zap { namespace graphics {

const char* const line_batch_vshdr = GLSL(
    uniform mat4 MVP;

    in vec3 position;
    in vec3 tangent;
    in ivec4 colour1;
    in vec2 texcoord1;
    in float pointsize;

    out vec2 tex;
    out vec4 col;

    void main() {
        vec4 P = MVP * vec4(position, 1.);
        vec4 T = MVP * vec4(tangent, 0.);
        vec2 V = P.xy / P.w;
        vec2 tn = normalize(T.xy);

        vec2 N = (texcoord1.y == 0. ? 1. : -1.) * vec2(-tn.y, tn.x);
        tex = texcoord1;
        col = vec4(colour1 / 255.);
        gl_Position = vec4(V.xy + pointsize * N, 0., 1.);
    }
);

const char* const line_batch_fshdr = GLSL(
    uniform sampler2D diffuse_tex;
    uniform int render_mode;        // 0 - flat colour, 1 - r8_t tex, 2 - rgb888_t or rgba8888_t tex

    in vec2 tex;
    in vec4 col;

    out vec4 frag_colour;
    void main() {
        switch(render_mode) {
            case 0:
                frag_colour = col;
                break;
            case 1:
                frag_colour = col * texture(diffuse_tex, tex).rrrr;
                break;
            case 2:
                frag_colour = col * texture(diffuse_tex, tex);
                break;
            default:
                frag_colour = col;
        }
    }
);

line_batch::line_batch() = default;

line_batch::~line_batch() = default;

const char* const uniform_arr[3] = {
        "MVP", "diffuse_tex", "render_mode"
};

bool line_batch::initialise(uint32_t line_count) {
    if(!batch_.initialise(4*line_count, 6*line_count, buffer_usage::BU_DYNAMIC_DRAW)) {
        LOG_ERR("Failed to initialise line_batch::render_batch");
        return false;
    }

    if(!prog_.link(line_batch_vshdr, line_batch_fshdr)) {
        LOG_ERR("Failed to link line_batch::program");
        return false;
    }

    prog_.bind();
    std::transform(uniform_arr, uniform_arr+3, uniforms_, [this](auto& key) { return prog_.uniform_location(key); });
    LOG(uniforms_[0], uniforms_[1], uniforms_[2]);
    prog_.bind_uniform(uniforms_[1], 0);       // Use texture unit 0
    prog_.release();

    return true;
}

uint32_t line_batch::create_line(const vec3f& A, const vec3f& B, const vec4b& colour, float width) {
    auto tok = batch_.allocate(primitive_type::PT_TRIANGLES, 4, 6);
    if(!tok) return uint32_t(-1);

    const auto D = normalise(B - A);

    std::vector<vertex_t> vertices = {
        vertex_t{A, D, colour, vec2f{0.f, 0.f}, width},
        vertex_t{B, D, colour, vec2f{1.f, 0.f}, width},
        vertex_t{B, D, colour, vec2f{1.f, 1.f}, width},
        vertex_t{A, D, colour, vec2f{0.f, 1.f}, width}
    };

    if(!batch_.load(tok, vertices, { 0, 2, 1, 0, 3, 2 })) {
        LOG_ERR("Failed to load line");
        batch_.free(tok);
        return uint32_t(-1);
    }

    const auto id = segments_.size();
    segments_.push_back(tok);
    return uint32_t(id);
}

uint32_t line_batch::create_line(const std::vector<vec3f>& points, const vec4b& colour, float width) {
    auto tok = batch_.allocate(primitive_type::PT_TRIANGLES, 4*points.size(), 6*points.size());
    if(!tok) return uint32_t(-1);

    return uint32_t(-1);
}

bool line_batch::update_line(uint32_t id, const vec3f& A, const vec3f& B, const vec4b& colour, float width) {
    return false;
}

bool line_batch::update_line(uint32_t id, const std::vector<vec3f>& points, const vec4b& colour, float width) {
    return false;
}

void line_batch::draw(uint32_t id, const mat4f& MVP) {
    batch_.bind();
    prog_.bind();
    prog_.bind_uniform(uniforms_[0], MVP);
    prog_.bind_uniform(uniforms_[2], 0);       // Render mode 0, colour only, no texture
    batch_.draw(segments_[id]);
    prog_.release();
    batch_.release();
}

}}