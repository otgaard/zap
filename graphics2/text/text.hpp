/* Created by Darren Otgaar on 2017/09/22. http://www.github.com/otgaard/zap */
#ifndef ZAP_TEXT_HPP
#define ZAP_TEXT_HPP

#if defined(FOUND_FREETYPE)

#include <string>

namespace zap { namespace graphics {

    class font;

    class text {
    public:
        text(font* font_ptr, const std::string& str, size_t len=0);
        ~text();

        void set_font(font* font_ptr);
        void set_text(const std::string& str);
        font* get_font() const;
        const std::string& get_text() const;
        size_t size() const;

    private:
        void build_string(const std::string& str);
    };

}}
#endif //defined(FOUND_FREETYPE)
#endif //ZAP_TEXT_HPP
