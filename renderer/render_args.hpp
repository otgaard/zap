//
// Created by Darren Otgaar on 2017/08/18.
//

#ifndef ZAP_RENDER_ARGS_HPP
#define ZAP_RENDER_ARGS_HPP

/**
 * render_args provide the method by which render_contexts are re-used.  The render_args are constructed as a structure
 * that overrides the default render_context binding (the master render_context).  The render_args are a block of memory
 * that can be passed to the render_context when drawing an instance so that:
 *
 * 1) All transformations for the target mesh are updated and available.
 * 2) All non-default parameters are bound before drawing.
 * 3) All non-default render_state is configured before drawing.
 * 4) All non-default state is restored after drawing (optional).
 */

#include <renderer/render_context.hpp>

namespace zap { namespace renderer {
    class render_args {
    public:
        render_args(render_context* context);
        render_args(const render_args& rhs);
        render_args(render_args&& rhs) noexcept;
        ~render_args();

        render_args& operator=(const render_args& rhs);
        render_args& operator=(render_args&& rhs) noexcept;

    private:
        render_context* context_;
        // Temporary, move to custom allocator
        std::vector<char> arg_buffer_;
    };
}}

#endif //ZAP_RENDER_ARGS_HPP
