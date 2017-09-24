/* Created by Darren Otgaar on 2017/09/18. http://www.github.com/otgaard/zap */
#ifndef ZAP_FONT_HPP
#define ZAP_FONT_HPP

// Note:  This class relies on the Freetype2 library
#if defined(FOUND_FREETYPE)

#include <array>
#include <string>
#include <vector>
#include <maths/geometry/rect.hpp>

namespace zap {
    namespace engine {
        class texture;
    }
    namespace renderer {
        class camera;
    }
}

namespace zap { namespace graphics {
    using recti = zap::maths::geometry::recti;
    using rectf = zap::maths::geometry::rectf;

    struct glyph {
        long advance = 0;
        recti bound;
        rectf texcoord;
    };

    using glyph_set = std::array<glyph, 0xFF>;

    class text_batcher;

    struct font {
        uint32_t font_id;
        std::string name;
        int px_height = 0;
        text_batcher* parent;

        explicit font(uint32_t font_id=INVALID_IDX, text_batcher* parent=nullptr) : font_id(font_id), parent(parent) { }
        font(const font&) = default;
        font& operator=(const font&) = default;
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

        void draw(const renderer::camera& cam);

        uint32_t font_count();
        font* add_font(const std::string& path, int px_height);
        font* get_font(uint32_t font_id);
        font* find_font(const std::string& name);
        texture* get_texture(uint32_t font_id);

        text create_text(uint32_t font_id, const std::string& str, uint32_t max_len=0);
        bool change_text(uint32_t text_id, const std::string& str, uint32_t max_len=0);
        void destroy_text(uint32_t text_id);
        void destroy_text(const text& txt);
        void translate_text(uint32_t text_id, int x, int y);

        font* get_text_font(uint32_t text_id);
        const std::string& get_text_string(uint32_t text_id);
        size_t get_text_size(uint32_t text_id);

    private:
        glyph get_glyph(uint32_t font_id, char ch);

        struct state_t;
        std::unique_ptr<state_t> state_;
        state_t& s;
    };
}}

#endif //defined(FOUND_FREETYPE)
#endif //ZAP_FONT_HPP
