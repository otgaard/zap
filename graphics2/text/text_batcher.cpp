/* Created by Darren Otgaar on 2017/09/18. http://www.github.com/otgaard/zap */

#if defined(FOUND_FREETYPE)

#include "text_batcher.hpp"
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include "text.hpp"
#include <algorithm>
#include <tools/log.hpp>
#include <engine/pixel_format.hpp>
#include <engine/pixmap.hpp>
#include <engine/texture.hpp>
#include <graphics2/g2_types.hpp>
#include <engine/program.hpp>
#include <renderer/camera.hpp>

using namespace zap;
using namespace zap::engine;
using namespace zap::graphics;
using namespace zap::renderer;

const int MAX_FONTS = 32;                           // Reserved Total number of supported fonts
const int CHARSET_SIZE = 0xFF;                      // Number of Characters in charset (use ASCII codes for now)
const int CHARSET_DIM = 16;                         // Store 16 characters per dimension (16 x 16 = 256)
const int STRING_RESERVE = 128;                     // 128 Strings
const int CHAR_RESERVE = 1024 * 10;                 // 10k chars to start
const int CHAR_VTX_RESERVE = 4 * CHAR_RESERVE;      // 40k vertex space (1 quad per char)
const int CHAR_IDX_RESERVE = 6 * CHAR_RESERVE;      // 60k index space (2 * tri per char)

using vstream = vertex_stream<vbuf_p2t2_t>;

struct text_string {
    uint32_t font_id;
    uint32_t start_idx;         // zero-indexed index_buffer start
    uint32_t last_idx;          // zero-indexed index_buffer last_used quad
    uint32_t end_idx;           // zero-indexed index_buffer end (total space)
    uint32_t size;              // total chars used in text
    uint32_t reserved;          // total chars available
    vec2i translation;          // the string translation in orthographic space for now
    std::string text;
};

size_t count_quads(const std::string& str) {
    return (size_t)std::count_if(str.begin(), str.end(), [](char ch)->bool {
        return !(ch == ' ' || ch == '\n' || ch == '\t' || ch == '\v');
    });
}

struct text_batcher::state_t {
    bool initialised = false;
    FT_Library library;
    program shdr_prog;

    vbuf_p2t2_t vbuffer{};
    ibuf_tri4_t ibuffer{};
    mesh_p2t2_tri4_t batch{vstream{&vbuffer}, &ibuffer};

    std::vector<text_string> batch_index;

    std::vector<texture> textures;
    std::vector<font> fonts;
    std::vector<glyph_set> glyph_sets;

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

    out vec4 frag_colour;

    void main() {
        frag_colour = texture(text_atlas, tex).rrrr;
    }
);

zap::graphics::text_batcher::text_batcher() : state_(new state_t{}), s(*state_) {
}

zap::graphics::text_batcher::~text_batcher() {
    if(s.initialised) {
        auto err = FT_Done_FreeType(s.library);
        if(err) LOG_ERR("Failed to release FreeType2 Library");
    }
}

bool zap::graphics::text_batcher::initialise() {
    auto err = FT_Init_FreeType(&s.library);
    if(err) {
        LOG_ERR("Failed to initialise FreeType2 library");
        return false;
    }

    LOG("Allocating buffers", CHAR_RESERVE);

    s.vbuffer.usage(buffer_usage::BU_STREAM_DRAW);
    s.ibuffer.usage(buffer_usage::BU_STREAM_DRAW);

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

    // Reserve space in the vectors
    s.batch_index.reserve(STRING_RESERVE);
    s.fonts.reserve(MAX_FONTS);
    s.textures.resize(MAX_FONTS);

    s.initialised = true;
    return true;
}

void zap::graphics::text_batcher::draw(const renderer::camera& cam) {
    s.shdr_prog.bind();
    s.shdr_prog.bind_texture_unit("text_atlas", 0);
    s.batch.bind();
    s.textures[0].bind(0);
    for(uint32_t idx = 0; idx != s.batch_index.size(); ++idx) {
        auto& txt = s.batch_index[idx];
        s.shdr_prog.bind_uniform("pvm", cam.proj_view() * make_translation(float(txt.translation.x), float(txt.translation.y), 0.f));
        //LOG("el:", idx, "s:", txt.start_idx, "c:", txt.last_idx - txt.start_idx);
        s.batch.draw(txt.start_idx, txt.last_idx - txt.start_idx);
    }
    s.textures[0].release();
    s.batch.release();
    s.shdr_prog.release();
}

uint32_t zap::graphics::text_batcher::font_count() {
    return uint32_t(s.fonts.size());
}

zap::graphics::font* zap::graphics::text_batcher::add_font(const std::string& path, int px_height) {
    if(!s.initialised || s.fonts.size() == MAX_FONTS) return nullptr;

    FT_Face face{};
    auto err = FT_New_Face(s.library, path.c_str(), 0, &face);
    if(err) {
        LOG_ERR("FreeType2 failed to build font from file:", path);
        return nullptr;
    }

    err = FT_Set_Pixel_Sizes(face, 0, px_height);
    if(err) {
        LOG_ERR("FreeType2 failed to set face pixel size");
        return nullptr;
    }

    err = FT_Select_Charmap(face, FT_ENCODING_UNICODE);
    if(err) {
        LOG_ERR("FreeType2 failed to load the selected character map");
        return nullptr;
    }

    std::vector<FT_BitmapGlyph> glyphs;
    for(int i = 0; i != CHARSET_SIZE; ++i) {
        err = FT_Load_Char(face, i, FT_LOAD_TARGET_NORMAL); // TODO: Allow renderer hints to be passed in
        if(!err) {
            FT_Glyph glyph{};
            err = FT_Get_Glyph(face->glyph, &glyph);
            if(!err) {
                FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, 0, 1);
                FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;
                glyphs.push_back(bitmap_glyph);
            }
        } else {
            glyphs.push_back(nullptr);
        }
    }
    // Allocate the glyph_set
    s.glyph_sets.emplace_back();
    auto& glyph_set = s.glyph_sets.back();

    int tex_height = px_height * CHARSET_DIM, tex_width = tex_height;
    pixmap<r8_t> atlas_image{tex_width, tex_height};
    int curr_col = 0, curr_row = 0, max_height = 0;
    std::vector<recti> coords(CHARSET_SIZE);
    std::vector<int> row_top(tex_width, 0);

    int idx = 0;
    for(auto& g : glyphs) {
        if(g == nullptr) continue;
        auto& curr = glyph_set[idx];
        auto& bm = g->bitmap;

        if(curr_col + (int)bm.width + 1 >= tex_width) {
            curr_col = 0;
        }

        curr_row = row_top[curr_col];
        for(int x = 0, end = bm.width+1; x < end; ++x) {
            curr_row = std::max(curr_row, row_top[curr_col+x]);
        }
        curr_row++;

        if(curr_row + (int)bm.rows+1 >= tex_height) {
            tex_height *= 2;
            atlas_image.resize(tex_width, tex_height);
        }

        curr.bound.left = g->left;
        curr.bound.top = -g->top;
        curr.bound.right = g->left + bm.width;
        curr.bound.bottom = bm.rows - g->top;
        curr.advance = g->root.advance.x >> 16;

        coords[idx] = recti(curr_col+1, curr_col+bm.width+1, curr_row+bm.rows+1, curr_row+1);

        const byte* px = bm.buffer;
        for(uint32_t r = 0; r != bm.rows; ++r) {
            for(uint32_t c = 0; c != bm.width; ++c) {
                auto i = c + curr_col + 1 + (r + curr_row + 1) * tex_width;
                atlas_image[i].set(px[c]);
            }
            px += bm.pitch;
        }

        for(int x = 0, end = bm.width+1; x != end; ++x) {
            row_top[curr_col+x] = curr_row + bm.rows;
        }

        curr_col += bm.width + 1;

        if(curr_row + (int)bm.rows > max_height) {
            max_height = curr_row + bm.rows;
        }

        FT_Done_Glyph((FT_Glyph)g);

        idx++;
    }

    uint32_t font_id = uint32_t(s.fonts.size());
    s.fonts.emplace_back(font{font_id, this});
    auto font_ptr = &s.fonts[font_id];
    font_ptr->px_height = px_height;
    font_ptr->name = path;
    s.textures.emplace_back();
    auto& tex = s.textures[font_id];

    tex_height = max_height + 1;
    atlas_image.resize(tex_width, tex_height);
    if(!tex.allocate() || !tex.initialise(atlas_image)) {
        LOG_ERR("Failed to initialise texture atlas for font:", path);
        s.glyph_sets.erase(s.glyph_sets.end()-1);
        s.fonts.erase(s.fonts.end()-1);
        s.textures.erase(s.textures.end()-1);
        return nullptr;
    }

    float inv_width = 1.f/tex_width, inv_height = 1.f/tex_height;

    for(int i = 0; i != CHARSET_SIZE; ++i) {
        glyph_set[i].texcoord = rectf{
                coords[i].left * inv_width,
                coords[i].right * inv_width,
                coords[i].bottom * inv_height,
                coords[i].top * inv_height
        };
    }

    return font_ptr;
}

font* zap::graphics::text_batcher::get_font(uint32_t font_id) {
    return font_id < s.fonts.size() ? &s.fonts[font_id] : nullptr;
}

font* zap::graphics::text_batcher::find_font(const std::string& name) {
    auto it = std::find_if(s.fonts.begin(), s.fonts.end(), [name](const auto& el) { return el.name == name; });
    return it != s.fonts.end() ? &(*it) : nullptr;
}

texture* text_batcher::get_texture(uint32_t font_id) {
    return font_id < s.fonts.size() ? &s.textures[font_id] : nullptr;
}

// TODO: 1) Organise free store according to font
// TODO: 2) Defragment free store

// The following could be implemented far better by using instanced rendering, but base offset instanced drawing
// isn't available in OpenGL 3.3

text text_batcher::create_text(uint32_t font_id, const std::string& str, uint32_t max_len) {
    text txt{};
    auto font_ptr = get_font(font_id);
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
            const auto& curr_glyph = get_glyph(font_id, ch);

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

        gl_error_check();

        s.vbuffer.unmap();
        s.ibuffer.unmap();
        s.vbuffer.release();
        s.ibuffer.release();
    } else {
        s.vbuffer.release();
        s.ibuffer.release();
        return txt;
    }

    auto text_id = uint32_t(s.batch_index.size());
    s.batch_index.emplace_back();
    auto& text_obj = s.batch_index[text_id];
    text_obj.font_id = font_id;
    text_obj.start_idx = s.index_ptr;
    text_obj.last_idx = s.index_ptr + 6 * char_len;
    text_obj.end_idx = s.index_ptr + idx_count;
    text_obj.size = char_len;
    text_obj.reserved = char_count;
    text_obj.translation = vec2i{0, 0};
    txt.set_fields(text_id, this);

    LOG(LOG_GREEN, "ENTRY:", text_obj.start_idx, text_obj.last_idx, text_obj.end_idx, text_obj.size);

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

void text_batcher::destroy_text(const text& txt) {

}

void text_batcher::translate_text(uint32_t text_id, int x, int y) {
    if(text_id < s.batch_index.size()) s.batch_index[text_id].translation.set(x, y);

}

font* text_batcher::get_text_font(uint32_t text_id) {
    return text_id < s.batch_index.size() ? &s.fonts[s.batch_index[text_id].font_id] : nullptr;
}

const std::string& text_batcher::get_text_string(uint32_t text_id) {
    static std::string str{};
    return str;
}

size_t text_batcher::get_text_size(uint32_t text_id) {
    return text_id < s.batch_index.size() ? s.batch_index[text_id].size : 0;
}

glyph text_batcher::get_glyph(uint32_t font_id, char ch) {
    return font_id < font_count() ? s.glyph_sets[font_id][ch] : glyph{};
}

#endif //defined(FOUND_FREETYPE)