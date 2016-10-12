/* Created by Darren Otgaar on 2016/10/12. http://www.github.com/otgaard/zap */
#ifndef ZAP_RENDER_STATE_HPP
#define ZAP_RENDER_STATE_HPP

#include <maths/algebra.hpp>

namespace zap { namespace engine { namespace renderer {
    class render_state {
    public:
        struct alpha_state { };
        struct cull_state { };
        struct scissor_state { };
        struct depth_state { };
        struct stencil_state { };
        struct multisample_state { };
        struct wire_state { };

    protected:

    };
}}}

#endif //ZAP_RENDER_STATE_HPP
