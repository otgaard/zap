/* Created by Darren Otgaar on 2017/09/18. http://www.github.com/otgaard/zap */
#ifndef ZAP_FONT_HPP
#define ZAP_FONT_HPP

// Note:  This class relies on the Freetype2 library
#if defined(FOUND_FREETYPE)

#include <array>
#include <string>
#include <maths/geometry/rect.hpp>

namespace zap { namespace engine {
    class texture;
}}

namespace zap { namespace graphics {
    using recti = zap::maths::geometry::recti;
    using rectf = zap::maths::geometry::rectf;

    struct glyph {
        long advance = 0;
        recti bound;
        rectf texcoord;
    };

    class text_batcher;

    struct font {
        int font_id;
        std::string name;
        int pixel_height;
        text_batcher* parent;

        font(int font_id=-1, text_batcher* parent=nullptr) : font_id(font_id), parent(parent) { }
    };

    class text;

    // The text_batcher controls batching and rendering of multiple on-screen text strings with fonts stored as
    // texture atlases with matching texcoord, boundary and advance instructions from Freetype.
    class text_batcher {
    public:
        using texture = engine::texture;

        text_batcher();
        ~text_batcher();

        bool initialise();

        int font_count();
        font* add_font(const std::string& path, int px_height);
        font* get_font(int font_id);
        font* find_font(const std::string& name);
        texture* get_texture(int font_id);

    private:
        struct state_t;
        std::unique_ptr<state_t> state_;
        state_t& s;
    };
}}

#endif //defined(FOUND_FREETYPE)
#endif //ZAP_FONT_HPP
