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
        uint32_t font_id;
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

        uint32_t font_count();
        font* add_font(const std::string& path, int px_height);
        font* get_font(uint32_t font_id);
        font* find_font(const std::string& name);
        texture* get_texture(uint32_t font_id);

        text create_text(uint32_t font_id, const std::string& str, size_t max_len=0);
        bool change_text(uint32_t text_id, const std::string& str, size_t max_len=0);
        void destroy_text(uint32_t text_id);
        void destroy_text(const text& txt);

        font* get_text_font(uint32_t text_id);
        const std::string& get_text_string(uint32_t text_id);
        size_t get_text_size(uint32_t text_id);

    private:
        struct state_t;
        std::unique_ptr<state_t> state_;
        state_t& s;
    };
}}

#endif //defined(FOUND_FREETYPE)
#endif //ZAP_FONT_HPP
