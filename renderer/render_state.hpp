/* Created by Darren Otgaar on 2016/10/12. http://www.github.com/otgaard/zap */
#ifndef ZAP_RENDER_STATE_HPP
#define ZAP_RENDER_STATE_HPP

#include <maths/algebra.hpp>
#include <vector>

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
        std::vector<char> state_storage_;
        alpha_state* alpha_state_;
        cull_state* cull_state_;
        scissor_state* scissor_state_;
        depth_state* depth_state_;
        stencil_state* stencil_state_;
        multisample_state* multisample_state_;
        wire_state* wire_state_;
    };
}}}

#endif //ZAP_RENDER_STATE_HPP
