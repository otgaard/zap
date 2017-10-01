/* Created by Darren Otgaar on 2017/09/30. http://www.github.com/otgaard/zap */

#if defined(FOUND_FREETYPE)

#include "font_manager.hpp"
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

using namespace zap;
using namespace zap::engine;
using namespace zap::graphics;

const int MAX_FONTS = 32;                           // Reserved Total number of supported fonts
const int CHARSET_DIM = 16;                         // Store 16 characters per dimension (16 x 16 = 256)

const glyph_set* font::get_glyphs() const {
    return parent->get_glyphs(font_id);
}

const font::atlas_t* font::get_atlas() const {
    return parent->get_atlas(font_id);
}

const glyph& font::get_glyph(byte ch) const {
    return parent->get_glyph(font_id, ch);
}

struct font_manager::state_t {
    bool initialised = false;
    FT_Library library;
    std::vector<font> fonts;
    std::vector<glyph_set> glyph_sets;
    std::vector<pixmap_t> atlases;
};

font_manager::font_manager() : state_(new state_t{}), s(*state_) {
}

font_manager::~font_manager() {
    if(s.initialised) {
        auto err = FT_Done_FreeType(s.library);
        if(err) LOG_ERR("Failed to release FreeType2 Library");
    }
}

bool font_manager::initialise() {
    auto err = FT_Init_FreeType(&s.library);
    if(err) {
        LOG_ERR("Failed to initialise FreeType2 Library");
        return false;
    }

    s.fonts.reserve(MAX_FONTS);
    s.atlases.reserve(MAX_FONTS);
    s.initialised = true;
    return true;
}

uint32_t font_manager::font_count() const {
    return uint32_t(s.fonts.size());
}

const font* font_manager::add_font(const std::string& path, int px_height) {
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

    err = FT_Done_Face(face);
    if(err) LOG_ERR("Failed to deallocate FreeType2 Font Face");

    tex_height = max_height + 1;
    atlas_image.resize(tex_width, tex_height);
    s.atlases.emplace_back(std::move(atlas_image));

    auto font_id = uint32_t(s.fonts.size());
    s.fonts.emplace_back(font{font_id, this});
    auto font_ptr = &s.fonts[font_id];
    font_ptr->px_height = px_height;
    font_ptr->name = path;

    float inv_width = 1.f/tex_width, inv_height = 1.f/tex_height;

    for(int i = 0; i != CHARSET_SIZE; ++i) {
        // In inverted bitmap space
        glyph_set[i].coord = recti{coords[i].left, coords[i].right, coords[i].top, coords[i].bottom};
        glyph_set[i].texcoord = rectf{
                coords[i].left * inv_width,
                coords[i].right * inv_width,
                coords[i].bottom * inv_height,
                coords[i].top * inv_height
        };
    }

    return font_ptr;
}

const font* font_manager::get_font(uint32_t font_id) const {
    return font_id < font_count() ? &s.fonts[font_id] : nullptr;
}

const font* font_manager::find_font(const std::string& name) const {
    auto it = std::find_if(s.fonts.begin(), s.fonts.end(), [name](const auto& el) { return el.name == name; });
    return it != s.fonts.end() ? &(*it) : nullptr;
}

const glyph_set* font_manager::get_glyphs(uint32_t font_id) const {
    return font_id < font_count() ? &s.glyph_sets[font_id] : nullptr;
}

const font_manager::pixmap_t* font_manager::get_atlas(uint32_t font_id) const {
    return font_id < font_count() ? &s.atlases[font_id] : nullptr;
}

const glyph& font_manager::get_glyph(uint32_t font_id, byte ch) const  {
    static const glyph dummy{};
    return font_id < font_count() ? s.glyph_sets[font_id][ch] : dummy;
}

recti font_manager::metrics(uint32_t font_id, const std::string& txt) const {
    auto font_ptr = get_font(font_id);
    if(!font_ptr || txt.empty()) recti{0, 0, 0, 0};

    const auto px_height = font_ptr->px_height;

    int x = 0, y = px_height, max_x = 0, max_y = 0;
    uint32_t quad = 0;
    for(auto c = txt.begin(); c != txt.end(); ++c) {
        auto ch = *c;
        const auto& curr_glyph = font_ptr->get_glyph(ch);

        switch(ch) {
            case ' ':  x += curr_glyph.advance; continue;
            case '\n': y += px_height; if(x > max_x) max_x = x; x = 0; continue;
            case '\t': x += 4 * curr_glyph.advance; continue;
            case '\v': y += 4 * px_height; continue;
            default:   break;
        }

        auto curr_y = y + curr_glyph.bound.bottom;
        if(curr_y > max_y) max_y = curr_y;

        x += curr_glyph.advance;
        ++quad;
    }

    if(x > max_x) max_x = x;
    return recti{0, max_x, 0, max_y};
}

pixmap<rgb888_t> font_manager::rasterise(uint32_t font_id, const std::string& txt, const vec3b& fore, const vec3b& back) const {
    auto font_ptr = get_font(font_id);
    if(!font_ptr || txt.empty()) return pixmap<rgb888_t>{};

    auto txt_bound = metrics(font_id, txt);

    auto atlas_ptr = font_ptr->get_atlas();
    const auto px_height = font_ptr->px_height;

    pixmap<rgb888_t> img{txt_bound.width(), txt_bound.height()};
    img.clear(rgb888_t{back});

    // Conversion Function
    auto conv_fnc = [&fore, &back](const r8_t& i) -> rgb888_t {
        return rgb888_t{lerp(i.get1(), back, fore)};
    };

    int x = 0, y = px_height;
    uint32_t quad = 0;
    for(auto c = txt.begin(); c != txt.end(); ++c) {
        auto ch = *c;
        const auto& curr_glyph = font_ptr->get_glyph(ch);

        switch(ch) {
            case ' ':  x += curr_glyph.advance; continue;
            case '\n': y += px_height; x = 0; continue;
            case '\t': x += 4 * curr_glyph.advance; continue;
            case '\v': y += 4 * px_height; continue;
            default:   break;
        }

        img.copy(*atlas_ptr, x+curr_glyph.bound.left, y+curr_glyph.bound.top, curr_glyph.coord, conv_fnc);

        x += curr_glyph.advance;
        ++quad;
    }

    img.flip_y();
    return img;
}

#endif //defined(FOUND_FREETYPE)