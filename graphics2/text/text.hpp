/* Created by Darren Otgaar on 2017/09/22. http://www.github.com/otgaard/zap */
#ifndef ZAP_TEXT_HPP
#define ZAP_TEXT_HPP

#if defined(FOUND_FREETYPE)

#include <string>
#include <core/core.hpp>
#include <maths/vec2.hpp>

namespace zap { namespace graphics {

    struct font;
    class text_batcher;

    class text {
    public:
        using vec2i = maths::vec2i;

        ~text();

        bool is_allocated() const { return id_ != INVALID_IDX && parent_ != nullptr; }
        uint32_t get_id() const { return id_; }

        void translate(int x, int y);
        void translate(const vec2i& v) { translate(v.x, v.y); }

        void set_text(const std::string& str, size_t max_len=0);
        font* get_font() const;
        const std::string& get_text() const;
        size_t size() const;

    private:
        friend class text_batcher;

        void set_fields(uint32_t id, text_batcher* parent) { id_ = id; parent_ = parent; }

        text() = default;
        text(uint32_t id, text_batcher* parent);

        uint32_t id_ = INVALID_IDX;
        text_batcher* parent_ = nullptr;
    };

}}

#endif //defined(FOUND_FREETYPE)
#endif //ZAP_TEXT_HPP
