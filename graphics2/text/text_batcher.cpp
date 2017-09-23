/* Created by Darren Otgaar on 2017/09/18. http://www.github.com/otgaard/zap */

#if defined(FOUND_FREETYPE)

#include "text_batcher.hpp"
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include "text.hpp"
#include <tools/log.hpp>
#include <array>
#include <vector>
#include <engine/pixel_format.hpp>
#include <engine/pixmap.hpp>
#include <engine/texture.hpp>
#include <graphics2/g2_types.hpp>

using namespace zap;
using namespace zap::engine;
using namespace zap::graphics;

const int MAX_FONTS = 32;           // Reserved Total number of supported fonts
const int CHARSET_SIZE = 0xFF;      // Number of Characters in charset (use ASCII codes for now)
const int CHARSET_DIM = 16;         // Store 16 characters per dimension (16 x 16 = 256)
const int CHAR_RESERVE = 1024 * 10; // 10k text space to start

using glyph_set = std::array<glyph, CHARSET_SIZE>;

using vstream = vertex_stream<vbuf_p2t2_t>;

struct zap::graphics::text_batcher::state_t {
    bool initialised = false;
    FT_Library library;
    std::vector<font> fonts;
    std::vector<glyph_set> glyph_sets;
    std::vector<texture> textures;

    vbuf_p2t2_t vbuffer{};
    ibuf_tri4_t ibuffer{};
    mesh_p2t2_tri4_t batch{vstream{&vbuffer}, &ibuffer};
};

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

    // Reserve space in the vectors
    s.fonts.reserve(MAX_FONTS);
    s.textures.resize(MAX_FONTS);

    s.initialised = true;
    return true;
}

uint32_t zap::graphics::text_batcher::font_count() {
    return s.fonts.size();
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

        if(curr_col + bm.width + 1 >= tex_width) {
            curr_col = 0;
            curr_row += px_height;
        }

        curr_row = row_top[curr_col];
        for(int x = 0, end = bm.width+1; x < end; ++x) {
            curr_row = std::max(curr_row, row_top[curr_col+x]);
        }
        curr_row++;

        if(curr_row + bm.rows+1 >= tex_height) {
            tex_height *= 2;
            atlas_image.resize(tex_width, tex_height);
        }

        curr.bound.left = g->left;
        curr.bound.top = -g->top;
        curr.bound.right = g->left + bm.width;
        curr.bound.bottom = bm.rows - g->top;
        curr.advance = g->root.advance.x >> 16;

        coords[idx] = recti(curr_col+1, curr_row+1, curr_col+bm.width, curr_row+bm.rows+1);

        const byte* px = bm.buffer;
        for(int r = 0; r != bm.rows; ++r) {
            for(int c = 0; c != bm.width; ++c) {
                auto i = c + curr_col + 1 + (r + curr_row + 1) * tex_width;
                atlas_image[i].set(px[c]);
            }
            px += bm.pitch;
        }

        for(int x = 0, end = bm.width+1; x != end; ++x) {
            row_top[curr_col+x] = curr_row + bm.rows;
        }

        curr_col += bm.width + 1;

        if(curr_row + bm.rows > max_height) {
            max_height = curr_row + bm.rows;
        }

        FT_Done_Glyph((FT_Glyph)g);

        idx++;
    }

    auto font_id = (int)s.fonts.size();
    s.fonts.emplace_back(font{font_id, this});
    auto font_ptr = &s.fonts[font_id];
    font_ptr->pixel_height = px_height;
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

    for(int i = 0; i != CHARSET_SIZE; ++i) {
        glyph_set[i].texcoord = rectf{
                float(coords[i].left) / tex_width,
                float(coords[i].top) / tex_height,
                float(coords[i].right+1) / tex_width,
                float(coords[i].bottom) / tex_height
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


#endif //defined(FOUND_FREETYPE)