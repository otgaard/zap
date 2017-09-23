/* Created by Darren Otgaar on 2017/09/22. http://www.github.com/otgaard/zap */
#ifndef ZAP_TEXT_HPP
#define ZAP_TEXT_HPP

#if defined(FOUND_FREETYPE)

#include <string>

namespace zap { namespace graphics {

    struct font;
    class text_batcher;

    class text {
    public:
        ~text();

        bool is_allocated() const { return id_ != -1 && parent_ != nullptr; }
        uint32_t get_id() const { return id_; }

        void set_text(const std::string& str, size_t max_len=0);
        font* get_font() const;
        const std::string& get_text() const;
        size_t size() const;

    private:
        friend class text_batcher;
        text(uint32_t id, text_batcher* parent);

        uint32_t id_;
        text_batcher* parent_;
    };

}}

#endif //defined(FOUND_FREETYPE)
#endif //ZAP_TEXT_HPP
