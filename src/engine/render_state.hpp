/* Created by Darren Otgaar on 2016/10/12. http://www.github.com/otgaard/zap */
#ifndef ZAP_RENDER_STATE_HPP
#define ZAP_RENDER_STATE_HPP

#include "engine.hpp"
#include <maths/algebra.hpp>
#include <vector>

namespace zap { namespace engine {
    class ZAPENGINE_EXPORT render_state {
    public:
        explicit render_state(bool blend, bool depth, bool rasterisation) {
            initialise(blend, depth, rasterisation);
        }

        void initialise(bool blend, bool depth, bool rasterisation) {
            size_t alloc = 0;
            if(blend) alloc += sizeof(blend_state);
            if(depth) alloc += sizeof(depth_state);
            if(rasterisation) alloc += sizeof(rasterisation_state);

            storage_.resize(alloc);

            size_t offset = 0;

            if(blend) {
                blend_state_ = reinterpret_cast<blend_state*>(storage_.data());
                blend_state_->init();
                offset += sizeof(blend_state);
            }

            if(depth) {
                depth_state_ = reinterpret_cast<depth_state*>(storage_.data()+offset);
                depth_state_->init();
                offset += sizeof(depth_state);
            }

            if(rasterisation) {
                rasterisation_state_ = reinterpret_cast<rasterisation_state*>(storage_.data()+offset);
                rasterisation_state_->init();
                offset += sizeof(rasterisation_state);
            }
        }

        ~render_state() = default;

        enum class compare_mode;

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

            bool enabled;
            src_blend_mode src_mode;
            dst_blend_mode dst_mode;
            maths::vec4f colour;

            // TODO: Move to allocator + constructor
            void init() {   // Default to alpha blending turned off
                enabled = false;
                src_mode = src_blend_mode::SBM_SRC_ALPHA;
                dst_mode = dst_blend_mode::DBM_ONE_MINUS_SRC_ALPHA;
                colour.set(0.f, 0.f, 0.f, 0.f);
            }
        };

        struct depth_state {
            bool enabled;
            bool writable;
            compare_mode cmp_mode;
            float clear_depth;

            // TODO: Move to allocator + constructor
            void init() { // Default to depth testing on
                enabled = true;
                writable = true;
                cmp_mode = compare_mode::CM_LEQUAL;
                clear_depth = 1.f;
            }
        };

        struct rasterisation_state {
            enum class polygon_mode {
                PM_POINT,
                PM_LINE,
                PM_FILL,
                PM_SIZE
            };

            float line_width;
            float point_size;
            bool enable_program_point_size;
            bool enable_vertex_point_size;
            bool enable_point_smooth;
            bool enable_line_smooth;
            bool enable_polygon_smooth;
            polygon_mode poly_mode;

            void init() {
                line_width = 1.f;
                point_size = 1.f;
                enable_program_point_size = true;
                enable_vertex_point_size = true;
                enable_point_smooth = false;
                enable_line_smooth = false;
                enable_polygon_smooth = false;
                poly_mode = polygon_mode::PM_FILL;
            }
        };


        struct cull_state { };
        struct scissor_state { };
        struct stencil_state { };
        struct multisample_state { };

        blend_state* get_blend_state() const { return blend_state_; }
        depth_state* get_depth_state() const { return depth_state_; }
        rasterisation_state* get_rasterisation_state() const { return rasterisation_state_; }

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

    protected:
        std::vector<char> storage_;
        blend_state* blend_state_ = nullptr;
        depth_state* depth_state_ = nullptr;
        cull_state* cull_state_ = nullptr;
        scissor_state* scissor_state_ = nullptr;
        stencil_state* stencil_state_ = nullptr;
        multisample_state* multisample_state_ = nullptr;
        rasterisation_state* rasterisation_state_ = nullptr;
    };
}}

#endif //ZAP_RENDER_STATE_HPP
