/* Created by Darren Otgaar on 2017/09/30. http://www.github.com/otgaard/zap */
#ifndef ZAP_FONT_MANAGER_HPP
#define ZAP_FONT_MANAGER_HPP

// Split off font management from text_batcher due to need to use it in canvas.

#if defined(FOUND_FREETYPE)

#include <string>
#include <algorithm>
#include <maths/vec3.hpp>
#include <engine/pixmap.hpp>
#include <graphics/graphics.hpp>
#include <maths/geometry/rect.hpp>

namespace zap { namespace graphics {
    using recti = zap::maths::geometry::recti;
    using rectf = zap::maths::geometry::rectf;

    struct glyph {
        long advance = 0;
        recti bound;
        recti coord;
        rectf texcoord;
    };

    const int CHARSET_SIZE = 0xFF;  // ASCII for now
    using glyph_set = std::array<glyph, CHARSET_SIZE>;

    class font_manager;

    struct ZAPGRAPHICS_EXPORT font {
        using atlas_t = engine::pixmap<engine::r8_t>;
        using rgb888_t = engine::rgb888_t;

        uint32_t font_id;
        std::string name;
        int px_height = 0;
        font_manager* parent;
        explicit font(uint32_t font_id=INVALID_IDX, font_manager* parent=nullptr) : font_id(font_id), parent(parent) { }
        font(const font&) = default;
        font& operator=(const font&) = default;

        const glyph_set* get_glyphs() const;
        const atlas_t* get_atlas() const;
        const glyph& get_glyph(byte ch) const;
    };

    inline size_t count_quads(const std::string& str) {
        return (size_t)std::count_if(str.begin(), str.end(), [](char ch)->bool {
            return !(ch == ' ' || ch == '\n' || ch == '\t' || ch == '\v');
        });
    }

    class ZAPGRAPHICS_EXPORT font_manager {
    public:
        using pixmap_t = engine::pixmap<engine::r8_t>;
        using rgb888_t = engine::rgb888_t;
        using vec3b = maths::vec3b;

        font_manager();
        ~font_manager();

        bool initialise();

        uint32_t font_count() const;
        const font* add_font(const std::string& path, int px_height);
        const font* get_font(uint32_t font_id) const;
        const font* find_font(const std::string& name) const;
        const glyph_set* get_glyphs(uint32_t font_id) const;
        const pixmap_t* get_atlas(uint32_t font_id) const;

        const glyph& get_glyph(uint32_t font_id, byte ch) const;

        recti metrics(uint32_t font_id, const std::string& txt) const;
        engine::pixmap<engine::rgb888_t> rasterise(uint32_t font_id, const std::string& txt,
                                                   const vec3b& foreground=vec3b{0, 0, 0},
                                                   const vec3b& background=vec3b{255,255,255}) const;

    protected:
        struct state_t;
        std::unique_ptr<state_t> state_;
        state_t& s;
    };
}}

#endif //defined(FOUND_FREETYPE)
#endif //ZAP_FONT_MANAGER_HPP
