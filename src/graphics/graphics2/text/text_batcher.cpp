/* Created by Darren Otgaar on 2017/09/18. http://www.github.com/otgaard/zap */

#if defined(FOUND_FREETYPE)

#include "text_batcher.hpp"
#include "text.hpp"
#include <unordered_map>
#include <engine/texture.hpp>
#include <graphics/graphics2/g2_types.hpp>
#include <engine/program.hpp>
#include <renderer/camera.hpp>
#include <graphics/graphics2/text/font_manager.hpp>

using namespace zap;
using namespace zap::engine;
using namespace zap::graphics;
using namespace zap::renderer;

const int STRING_RESERVE = 128;                     // 128 Strings
const int CHAR_RESERVE = 1024 * 10;                 // 10k chars to start
const int CHAR_VTX_RESERVE = 4 * CHAR_RESERVE;      // 40k vertex space (1 quad per char)
const int CHAR_IDX_RESERVE = 6 * CHAR_RESERVE;      // 60k index space (2 * tri per char)

using vstream = vertex_stream<vbuf_p2t2_t>;

struct text_string {
    uint32_t font_id;
    uint32_t start_idx;         // zero-indexed index_buffer start
    uint32_t idx_count;         // zero-indexed index_buffer [start_idx, start_idx + idx_count]
    uint32_t end_idx;           // zero-indexed index_buffer end (total space)
    uint32_t size;              // total chars used in text
    uint32_t reserved;          // total chars available
    vec2i translation;          // the string translation in orthographic space for now
    vec4f colour;               // flat colour for string
    std::string text;
};

struct text_batcher::state_t {
    font_manager* font_mgr_;

    program shdr_prog;

    vbuf_p2t2_t vbuffer{};
    ibuf_tri4_t ibuffer{};
    mesh_p2t2_tri4_t batch{vstream{&vbuffer}, &ibuffer};

    std::vector<text_string> batch_index;
    std::unordered_map<uint32_t, texture> textures;

    uint32_t quad_ptr = 0;      // Stores the end of the allocated strings in quads (will be replaced by a list of free blocks)
    uint32_t vertex_ptr = 0;    // Last vertex index
    uint32_t index_ptr = 0;     // Last index ptr
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

zap::graphics::text_batcher::text_batcher() : state_(new state_t{}), s(*state_) {
}

zap::graphics::text_batcher::~text_batcher() {
    gl_error_check();
}

bool zap::graphics::text_batcher::initialise(font_manager* font_mgr) {
    if(!font_mgr) {
        LOG_ERR("Text Batcher requires a valid font_manager");
        return false;
    }

    s.font_mgr_ = font_mgr;

    LOG("Allocating buffers", CHAR_RESERVE);

    s.vbuffer.usage(buffer_usage::BU_DYNAMIC_DRAW);
    s.ibuffer.usage(buffer_usage::BU_DYNAMIC_DRAW);

    if(!s.batch.allocate() || !s.vbuffer.allocate() || !s.ibuffer.allocate()) {
        LOG_ERR("Failed to allocate GPU resources for text_batcher");
        return false;
    }

    s.batch.bind(); s.vbuffer.bind();
    if(!s.vbuffer.initialise(CHAR_VTX_RESERVE)) {
        LOG_ERR("Failed to initialise text_batcher vertex_buffer");
        return false;
    }

    s.ibuffer.bind();
    if(!s.ibuffer.initialise(CHAR_IDX_RESERVE)) {
        LOG_ERR("Failed to initialise text_batcher index_buffer");
        return false;
    }

    s.batch.release();

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

void zap::graphics::text_batcher::draw(const renderer::camera& cam) {
    s.shdr_prog.bind();
    s.batch.bind();
    s.textures[0].bind(0);
    for(uint32_t idx = 0; idx != s.batch_index.size(); ++idx) {
        auto& txt = s.batch_index[idx];
        s.shdr_prog.bind_uniform("pvm", cam.proj_view() * make_translation(float(txt.translation.x), float(txt.translation.y), 0.f));
        s.shdr_prog.bind_uniform("colour", txt.colour);
        s.batch.draw(txt.start_idx, txt.idx_count);
    }
    s.textures[0].release();
    s.batch.release();
    s.shdr_prog.release();
}

const texture* text_batcher::get_texture(uint32_t font_id) const {
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

// TODO: 1) Organise free store according to font
// TODO: 2) Defragment free store

// The following could be implemented far better by using instanced rendering, but base offset instanced drawing
// isn't available in OpenGL 3.3

text text_batcher::create_text(uint32_t font_id, const std::string& str, uint32_t max_len) {
    text txt{};
    auto font_ptr = s.font_mgr_->get_font(font_id);
    if(!font_ptr || (str.empty() && max_len == 0)) return txt;

    const auto px_height = font_ptr->px_height;
    const auto char_len = (uint32_t)count_quads(str);
    const uint32_t char_count = std::max(char_len, max_len);
    const uint32_t quad_count = 4 * char_count;
    const uint32_t idx_count = 6 * char_count;

    LOG("Text Inp:", char_count, quad_count, idx_count);

    float x = 0.f, y = float(px_height);

    s.vbuffer.bind();
    s.ibuffer.bind();

    if(s.vbuffer.map(range_access::BA_MAP_WRITE, s.vertex_ptr, quad_count)
       && s.ibuffer.map(range_access::BA_MAP_WRITE, s.index_ptr, idx_count)) {
        uint32_t quad = 0;
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
            s.vbuffer[idx].position.set(x + curr_glyph.bound.left, y + curr_glyph.bound.top);
            s.vbuffer[idx].texcoord1.set(curr_glyph.texcoord.left, curr_glyph.texcoord.top);
            idx++;
            s.vbuffer[idx].position.set(x + curr_glyph.bound.left, y + curr_glyph.bound.bottom);
            s.vbuffer[idx].texcoord1.set(curr_glyph.texcoord.left, curr_glyph.texcoord.bottom);
            idx++;
            s.vbuffer[idx].position.set(x + curr_glyph.bound.right, y + curr_glyph.bound.bottom);
            s.vbuffer[idx].texcoord1.set(curr_glyph.texcoord.right, curr_glyph.texcoord.bottom);
            idx++;
            s.vbuffer[idx].position.set(x + curr_glyph.bound.right, y + curr_glyph.bound.top);
            s.vbuffer[idx].texcoord1.set(curr_glyph.texcoord.right, curr_glyph.texcoord.top);

            uint32_t iidx = 6 * quad; idx = s.vertex_ptr + 4 * quad;
            s.ibuffer[iidx++] = idx;   s.ibuffer[iidx++] = idx+1; s.ibuffer[iidx++] = idx+3;
            s.ibuffer[iidx++] = idx+1; s.ibuffer[iidx++] = idx+2; s.ibuffer[iidx]   = idx+3;

            x += curr_glyph.advance;
            ++quad;
        }

        s.vbuffer.unmap();
        s.ibuffer.unmap();
        s.vbuffer.release();
        s.ibuffer.release();
    } else {
        s.vbuffer.release();
        s.ibuffer.release();
        return txt;
    }

    if(gl_error_check()) return txt;

    auto text_id = uint32_t(s.batch_index.size());
    s.batch_index.emplace_back();
    auto& text_obj = s.batch_index[text_id];
    text_obj.font_id = font_id;
    text_obj.start_idx = s.index_ptr;
    text_obj.idx_count = 6 * char_len;
    text_obj.end_idx = s.index_ptr + idx_count;
    text_obj.size = char_len;
    text_obj.reserved = char_count;
    text_obj.translation = vec2i{0, 0};
    text_obj.text = str;
    txt.set_fields(text_id, this);

    s.quad_ptr += char_count;
    s.vertex_ptr += quad_count;
    s.index_ptr += idx_count;

    return txt;
}

bool text_batcher::change_text(uint32_t text_id, const std::string& str, uint32_t max_len) {
    return false;
}

void text_batcher::destroy_text(uint32_t text_id) {

}

void text_batcher::translate_text(uint32_t text_id, int x, int y) {
    if(text_id < s.batch_index.size()) s.batch_index[text_id].translation.set(x, y);
}

void text_batcher::set_text_colour(uint32_t text_id, float r, float g, float b, float a) {
    if(text_id < s.batch_index.size()) s.batch_index[text_id].colour.set(r, g, b, a);
}

const font* text_batcher::get_text_font(uint32_t text_id) const {
    return text_id < s.batch_index.size() ? s.font_mgr_->get_font(s.batch_index[text_id].font_id) : nullptr;
}

const std::string& text_batcher::get_text_string(uint32_t text_id) {
    static std::string str{};
    return str;
}

size_t text_batcher::get_text_size(uint32_t text_id) {
    return text_id < s.batch_index.size() ? s.batch_index[text_id].size : 0;
}

#endif //defined(FOUND_FREETYPE)