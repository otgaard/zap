/* Created by Darren Otgaar on 2017/09/22. http://www.github.com/otgaard/zap */
#ifndef ZAP_TEXT_HPP
#define ZAP_TEXT_HPP

#if defined(FOUND_FREETYPE)

#include <string>
#include <core/core.hpp>
#include <maths/vec2.hpp>
#include <maths/vec4.hpp>
#include <maths/geometry/rect.hpp>
#include <graphics/graphics.hpp>

namespace zap { namespace graphics {

struct font;
class text_batcher;

class ZAPGRAPHICS_EXPORT text {
public:
    using vec2i = maths::vec2i;
    using vec4f = maths::vec4f;
    using recti = maths::geometry::recti;

    text(text&& rhs) noexcept : id_(rhs.id_), parent_(rhs.parent_) { rhs.id_ = INVALID_IDX; }
    ~text();

    text& operator=(text&& rhs) noexcept {
        if(this != &rhs) {
            std::swap(id_, rhs.id_);
            std::swap(parent_, rhs.parent_);
        }
        return *this;
    }

    bool is_allocated() const { return id_ != INVALID_IDX; }
    uint32_t get_id() const { return id_; }

    void translate(int x, int y);
    void translate(const vec2i& v) { translate(v.x, v.y); }
    vec2i translation() const;

    void set_colour(float r, float g, float b, float a);
    void set_colour(const vec4f& c) { return set_colour(c.r, c.g, c.b, c.a); }
    vec4f get_colour() const;

    void set_text(const std::string& str, size_t max_len=0);
    const font* get_font() const;
    const std::string& get_text() const;
    size_t size() const;

    recti get_bound() const;

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
