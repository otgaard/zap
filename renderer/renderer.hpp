/* Created by Darren Otgaar on 2016/08/14. http://www.github.com/otgaard/zap */
#ifndef ZAP_RENDERER_HPP
#define ZAP_RENDERER_HPP

// The OpenGL renderer

namespace zap {
    class renderer {
    public:
        renderer() = default;
        ~renderer() = default;

        template <typename NodeT>
        void render(const NodeT& node) const {

        }

    };
}
#endif //ZAP_RENDERER_HPP
