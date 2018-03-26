/* Created by Darren Otgaar on 2017/09/18. http://www.github.com/otgaard/zap */
#ifndef ZAP_FONT_HPP
#define ZAP_FONT_HPP

// Note:  This class relies on the Freetype2 library
#if defined(FOUND_FREETYPE)

#include <string>
#include <maths/vec4.hpp>
#include <graphics/graphics.hpp>
#include <maths/geometry/rect.hpp>
#include <maths/geometry/AABB.hpp>

namespace zap {
    namespace engine {
        class texture;
    }
    namespace renderer {
        class camera;
    }
}

namespace zap { namespace graphics {
    struct font;
    class text;
    class font_manager;

    // The text_batcher controls batching and rendering of multiple on-screen text strings with fonts stored as
    // texture atlases with matching texcoord, boundary and advance instructions from Freetype.
    class ZAPGRAPHICS_EXPORT text_batcher {
    public:
        using texture = engine::texture;

        text_batcher();
        ~text_batcher();

        bool initialise(font_manager* font_mgr);

        void draw(const renderer::camera& cam);

        const texture* get_texture(uint32_t font_id) const;

        text create_text(uint32_t font_id, const std::string& str, uint32_t max_len=0);
        bool change_text(uint32_t text_id, const std::string& str, uint32_t max_len=0);
        void destroy_text(uint32_t text_id);
        void translate_text(uint32_t text_id, int x, int y);
        void set_text_colour(uint32_t text_id, float r, float g, float b, float a);

        maths::vec2i get_text_translation(uint32_t text_id) const;
        maths::geometry::recti get_AABB(uint32_t text_id) const;
        maths::vec4f get_text_colour(uint32_t text_id) const;
        const font* get_text_font(uint32_t text_id) const;
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
