/* Created by Darren Otgaar on 2016/10/12. http://www.github.com/otgaard/zap */
#ifndef ZAP_RENDER_STATE_HPP
#define ZAP_RENDER_STATE_HPP

#include <maths/algebra.hpp>
#include <vector>

namespace zap { namespace engine {
    class render_state {
    public:
        explicit render_state(bool blend) {
            size_t alloc = 0;
            if(blend) alloc += sizeof(blend_state);

            storage_.resize(alloc);
            if(blend) {
                blend_state_ = reinterpret_cast<blend_state*>(storage_.data());
                blend_state_->init();
            }
        }

        ~render_state() = default;

        struct blend_state {
            enum class src_blend_mode {
                SBM_ZERO,
                SBM_ONE,
                SBM_DST_COLOUR,
                SBM_ONE_MINUS_DST_COLOUR,
                SBM_SRC_ALPHA,
                SBM_ONE_MINUS_SRC_ALPHA,
                SBM_DST_ALPHA,
                SBM_ONE_MINUS_DST_ALPHA,
                SBM_SRC_ALPHA_SATURATE,
                SBM_CONSTANT_COLOUR,
                SBM_ONE_MINUS_CONSTANT_COLOUR,
                SBM_CONSTANT_ALPHA,
                SBM_ONE_MINUS_CONSTANT_ALPHA,
                SBM_SIZE
            };

            enum class dst_blend_mode {
                DBM_ZERO,
                DBM_ONE,
                DBM_SRC_COLOUR,
                DBM_ONE_MINUS_SRC_COLOUR,
                DBM_SRC_ALPHA,
                DBM_ONE_MINUS_SRC_ALPHA,
                DBM_DST_ALPHA,
                DBM_DST_COLOUR,
                DBM_ONE_MINUS_DST_ALPHA,
                DBM_CONSTANT_COLOUR,
                DBM_ONE_MINUS_CONSTANT_COLOUR,
                DBM_CONSTANT_ALPHA,
                DBM_ONE_MINUS_CONSTANT_ALPHA,
                DBM_SIZE
            };

            enum class compare_mode {
                CM_NEVER,
                CM_LESS,
                CM_EQUAL,
                CM_LEQUAL,
                CM_GREATER,
                CM_NOTEQUAL,
                CM_GEQUAL,
                CM_ALWAYS,
                CM_SIZE
            };

            bool blend_enabled;
            src_blend_mode src_mode;
            dst_blend_mode dst_mode;
            bool compare_enabled;
            compare_mode cmp_mode;
            float reference;
            maths::vec4f colour;

            // TODO: Move to allocator + constructor
            void init() {   // Default to alpha blending
                blend_enabled = true;
                src_mode = src_blend_mode::SBM_SRC_ALPHA;
                dst_mode = dst_blend_mode::DBM_ONE_MINUS_SRC_ALPHA;
                compare_enabled = false;
                cmp_mode = compare_mode::CM_NEVER;
                reference = 0.f;
                colour.set(0.f, 0.f, 0.f, 0.f);
            }
        };

        struct cull_state { };
        struct scissor_state { };
        struct depth_state { };
        struct stencil_state { };
        struct multisample_state { };
        struct wire_state { };

    protected:
        std::vector<char> storage_;
        blend_state* blend_state_ = nullptr;
        cull_state* cull_state_ = nullptr;
        scissor_state* scissor_state_ = nullptr;
        depth_state* depth_state_ = nullptr;
        stencil_state* stencil_state_ = nullptr;
        multisample_state* multisample_state_ = nullptr;
        wire_state* wire_state_ = nullptr;
    };
}}

#endif //ZAP_RENDER_STATE_HPP
