/* Created by Darren Otgaar on 2016/10/12. http://www.github.com/otgaard/zap */
#ifndef ZAP_STYLE_HPP
#define ZAP_STYLE_HPP

#include <engine/program.hpp>

namespace zap { namespace engine { namespace renderer {
    class style {
    public:
        style() = default;
        style(size_t passes) : pass_count_(passes), passes_(passes, nullptr) { }

    protected:
        size_t pass_count_;
        std::vector<program*> passes_;
    };
}}}

#endif //ZAP_STYLE_HPP
