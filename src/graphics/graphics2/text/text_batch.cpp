/* Created by Darren Otgaar on 2017/09/18. http://www.github.com/otgaard/zap */

#if defined(FOUND_FREETYPE)

#include "text_batch.hpp"
#include "text.hpp"
#include <unordered_map>
#include <engine/texture.hpp>
#include <graphics/graphics2/g2_types.hpp>
#include <engine/program.hpp>
#include <renderer/camera.hpp>
#include <graphics/graphics2/text/font_manager.hpp>
#include <maths/geometry/AABB.hpp>
#include <renderer/render_batch.hpp>

using namespace zap;
using namespace zap::engine;
using namespace zap::maths::geometry;
using namespace zap::graphics;
using namespace zap::renderer;

namespace zap { namespace graphics {

using render_batch_t = render_batch<vertex_stream<vbuf_p2t2_t>, ibuf_u32_t>;

const int STRING_RESERVE = 128;                     // 128 Strings
const int CHAR_RESERVE = 1024 * 10;                 // 10k chars to start
const int CHAR_VTX_RESERVE = 4 * CHAR_RESERVE;      // 40k vertex space (1 quad per char)
const int CHAR_IDX_RESERVE = 6 * CHAR_RESERVE;      // 60k index space (2 * tri per char)

struct text_string {
    uint32_t font_id;
    render_batch_t::token tok;  // token
    uint32_t size;              // total chars used in text
    uint32_t reserved;          // total chars available
    vec2i translation;          // the string translation in orthographic space for now
    vec4f colour;               // flat colour for string
    geometry::recti bound;      // bound in model coordinates
    std::string text;
};

struct text_batch::state_t {
    font_manager* font_mgr_;

    program shdr_prog;

    render_batch_t rndr_batch;
    std::vector<text_string> batch_index;

    std::unordered_map<uint32_t, texture> textures;
    bool is_bound = false;
    bool is_mapped = false;
};

const char* const text_vshdr = GLSL(
    in vec2 position;
    in vec2 texcoord1;

    out vec2 tex;

    uniform mat4 pvm;

    void main() {
        tex = texcoord1;
        gl_Position = pvm * vec4(position, 0., 1.);
    }
);

const char* const text_fshdr = GLSL(
    in vec2 tex;

    uniform sampler2D text_atlas;
    uniform vec4 colour;

    out vec4 frag_colour;

    void main() {
        frag_colour = colour * texture(text_atlas, tex).rrrr;
    }
);

text_batch::text_batch() : state_(new state_t{}), s(*state_) {
}

text_batch::~text_batch() {
    gl_error_check();
}

bool text_batch::initialise(font_manager* font_mgr) {
    if(!font_mgr) {
        LOG_ERR("Text Batcher requires a valid font_manager");
        return false;
    }

    s.font_mgr_ = font_mgr;

    LOG("Allocating buffers", CHAR_RESERVE);

    if(!s.rndr_batch.initialise(CHAR_VTX_RESERVE, CHAR_IDX_RESERVE, buffer_usage::BU_DYNAMIC_DRAW)) {
        LOG_ERR("Failed to initialise text_batcher::render_batch");
        return false;
    }

    s.shdr_prog.add_shader(shader_type::ST_VERTEX, text_vshdr);
    s.shdr_prog.add_shader(shader_type::ST_FRAGMENT, text_fshdr);
    if(!s.shdr_prog.link()) {
        LOG_ERR("Failed to build text_batcher shader program");
        return false;
    }

    s.shdr_prog.bind();
    s.shdr_prog.bind_texture_unit("text_atlas", 0);
    s.shdr_prog.release();

    // Reserve space in the vectors
    s.batch_index.reserve(STRING_RESERVE);
    return true;
}

void text_batch::bind() {
    s.shdr_prog.bind();
    s.textures[0].bind(0);
    s.rndr_batch.bind();
    s.is_bound = true;
}

void text_batch::release() {
    s.rndr_batch.release();
    s.textures[0].release();
    s.shdr_prog.release();
    s.is_bound = false;
}

void text_batch::draw(const renderer::camera& cam) {
    return draw(cam.proj_view());
}

void text_batch::draw(const maths::mat4f& vp) {
    bool was_bound = !s.is_bound;
    if(was_bound) bind();

    for(uint32_t idx = 0; idx != s.batch_index.size(); ++idx) {
        auto& txt = s.batch_index[idx];
        s.shdr_prog.bind_uniform("pvm", vp * make_translation(float(txt.translation.x), float(txt.translation.y), 0.f));
        s.shdr_prog.bind_uniform("colour", txt.colour);
        s.rndr_batch.draw(txt.tok);
    }

    if(was_bound) release();
}

void text_batch::draw(uint32_t text_id, const renderer::camera& cam) {
    draw(text_id, cam.proj_view());
}

void text_batch::draw(uint32_t text_id, const maths::mat4f& vp) {
    if(text_id >= s.batch_index.size()) return;

    bool was_bound = !s.is_bound;
    if(was_bound) bind();

    auto& txt = s.batch_index[text_id];
    s.shdr_prog.bind_uniform("pvm", vp * make_translation(float(txt.translation.x), float(txt.translation.y), 0.f));
    s.shdr_prog.bind_uniform("colour", txt.colour);
    s.rndr_batch.draw(txt.tok);

    if(was_bound) release();
}

const texture* text_batch::get_texture(uint32_t font_id) const {
    auto font_ptr = s.font_mgr_->get_font(font_id);
    if(!font_ptr) return nullptr;

    auto it = s.textures.find(font_id);
    if(it != s.textures.end()) return &it->second;
    else {
        s.textures.emplace(font_id, texture{});
        auto atlas_ptr = font_ptr->get_atlas();
        if(!s.textures[font_id].allocate() || !s.textures[font_id].initialise(*atlas_ptr)) {
            LOG_ERR("Failed to initialise texture atlas for font:", font_ptr->name);
            s.textures.erase(font_id);
            return nullptr;
        }
        return &s.textures[font_id];
    }
}

// The following could be implemented far better by using instanced rendering, but base offset instanced drawing
// isn't available in OpenGL 3.3

text text_batch::create_text(uint32_t font_id, const std::string& str, uint32_t max_len) {
    text txt{};
    auto font_ptr = s.font_mgr_->get_font(font_id);
    if(!font_ptr || (str.empty() && max_len == 0)) return txt;

    const auto px_height = font_ptr->px_height;
    const auto char_len = (uint32_t)count_quads(str);
    const uint32_t char_count = std::max(char_len, max_len);
    const uint32_t quad_count = 4 * char_count;
    const uint32_t idx_count = 6 * char_count;

    //LOG("Text Inp:", char_count, quad_count, idx_count);

    auto tok = s.rndr_batch.allocate(primitive_type::PT_TRIANGLES, quad_count, idx_count);

    vec2i top_left{0, px_height}, bottom_right{0, 0};
    int32_t x = 0, y = px_height;
    uint32_t quad = 0;

    if(s.rndr_batch.map_write(tok)) {
        for(auto c = str.begin(); c != str.end(); ++c) {
            auto ch = *c;
            const auto& curr_glyph = s.font_mgr_->get_glyph(font_id, ch);

            switch(ch) {
                case ' ':  x += curr_glyph.advance; continue;
                case '\n': y += px_height; x = 0; continue;
                case '\t': x += 4 * curr_glyph.advance; continue;
                case '\v': y += 4 * px_height; continue;
                default:   break;
            }

            uint32_t idx = 4 * quad;

            s.rndr_batch.set(tok, idx,
                vtx_p2t2_t{
                    vec2f{float(x) + curr_glyph.bound.left, float(y) + curr_glyph.bound.top},
                    vec2f{curr_glyph.texcoord.left, curr_glyph.texcoord.top}
            });

            s.rndr_batch.set(tok, idx+1,
                 vtx_p2t2_t{
                     vec2f{float(x) + curr_glyph.bound.left, float(y) + curr_glyph.bound.bottom},
                     vec2f{curr_glyph.texcoord.left, curr_glyph.texcoord.bottom}
             });

            s.rndr_batch.set(tok, idx+2,
                 vtx_p2t2_t{
                     vec2f{float(x) + curr_glyph.bound.right, float(y) + curr_glyph.bound.bottom},
                     vec2f{curr_glyph.texcoord.right, curr_glyph.texcoord.bottom}
             });

            s.rndr_batch.set(tok, idx+3,
                 vtx_p2t2_t{
                     vec2f{float(x) + curr_glyph.bound.right, float(y) + curr_glyph.bound.top},
                     vec2f{curr_glyph.texcoord.right, curr_glyph.texcoord.top}
             });

            s.rndr_batch.set(tok, 6*quad, {idx+0, idx+1, idx+3, idx+1, idx+2, idx+3});

            top_left.set(0, std::min(y + curr_glyph.bound.top, top_left.y));
            bottom_right.set(std::max(x + curr_glyph.bound.right, bottom_right.x), std::max(y + curr_glyph.bound.bottom, bottom_right.y));

            x += curr_glyph.advance;
            ++quad;
        }
        s.rndr_batch.unmap();
    }

    auto text_id = uint32_t(s.batch_index.size());
    s.batch_index.emplace_back();
    auto& text_obj = s.batch_index[text_id];
    text_obj.font_id = font_id;
    text_obj.tok = tok;
    text_obj.size = char_len;
    text_obj.reserved = char_count;
    text_obj.translation = vec2i{0, 0};
    text_obj.text = str;
    text_obj.bound = recti{top_left.x, bottom_right.x, top_left.y, bottom_right.y};
    txt.set_fields(text_id, this);

    return txt;

}

bool text_batch::change_text(uint32_t text_id, const std::string& str, uint32_t max_len) {
    return false;
}

void text_batch::destroy_text(uint32_t text_id) {

}

void text_batch::translate_text(uint32_t text_id, int x, int y) {
    if(text_id < s.batch_index.size()) s.batch_index[text_id].translation.set(x, y);
}

vec2i text_batch::get_text_translation(uint32_t text_id) const {
    return text_id < s.batch_index.size() ? s.batch_index[text_id].translation : vec2i{0, 0};
}

geometry::recti text_batch::get_AABB(uint32_t text_id) const {
    return text_id < s.batch_index.size() ? s.batch_index[text_id].bound : recti{0, 0, 0, 0};
}

void text_batch::set_text_colour(uint32_t text_id, float r, float g, float b, float a) {
    if(text_id < s.batch_index.size()) s.batch_index[text_id].colour.set(r, g, b, a);
}

vec4f text_batch::get_text_colour(uint32_t text_id) const {
    return text_id < s.batch_index.size() ? s.batch_index[text_id].colour : vec4f{0.f, 0.f, 0.f, 0.f};
}

const font* text_batch::get_text_font(uint32_t text_id) const {
    return text_id < s.batch_index.size() ? s.font_mgr_->get_font(s.batch_index[text_id].font_id) : nullptr;
}

const std::string& text_batch::get_text_string(uint32_t text_id) {
    static std::string str{};
    return str;
}

size_t text_batch::get_text_size(uint32_t text_id) {
    return text_id < s.batch_index.size() ? s.batch_index[text_id].size : 0;
}

}}

#endif //defined(FOUND_FREETYPE)