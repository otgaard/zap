//
// Created by Darren Otgaar on 2016/10/10.
//

#ifndef ZAP_VISUAL_HPP
#define ZAP_VISUAL_HPP

#include "spatial.hpp"
#include <engine/mesh.hpp>
#include <renderer/render_args.hpp>

namespace zap { namespace scene_graph {
    template<typename SpatialT>
    class visual : public SpatialT {
    public:
        using spatial_t = SpatialT;
        using primitive_type = zap::engine::primitive_type;
        using mesh_base = zap::engine::mesh_base;
        using render_args = zap::renderer::render_args;
        using render_context = zap::renderer::render_context;

        visual() = default;
        visual(primitive_type type, mesh_base* mesh, render_context* context) : type_(type), mesh_{mesh}, context_{context}, args_{context} { }
        visual(const visual& rhs) : spatial_t(rhs), type_(rhs.type_), mesh_{rhs.mesh_}, context_{rhs.context_}, args_{rhs.context_} { }
        visual(visual&& rhs) noexcept : spatial_t(rhs), type_(rhs.type_), mesh_{rhs.mesh_}, context_{rhs.context_}, args_{rhs.context_} { }

        visual& operator=(const visual& rhs) {
            if(this != &rhs) {
                spatial_t::operator=(rhs);
                type_ = rhs.type_;
                mesh_ = rhs.mesh_;
                context_ = rhs.context_;
                args_ = rhs.args_;
            }
            return *this;
        }

        visual& operator=(visual&& rhs) noexcept {
            if(this != &rhs) {
                spatial_t::operator=(std::move(rhs));
                type_ = rhs.type_;
                mesh_ = rhs.mesh_;
                context_ = rhs.context_;
                args_ = std::move(rhs.args_);
            }
            return *this;
        }

        bool is_valid() const { return mesh_ != nullptr && context_ != nullptr; }

        void set_type(primitive_type type) { type_ = type; }
        void set_mesh(mesh_base* mesh) { mesh_ = mesh; }
        void set_context(render_context* context) { context_ = context; }
        mesh_base* get_mesh() { return mesh_; }
        render_context* get_context() { return context_; }
        primitive_type get_type() const { return type_; }
        const mesh_base* get_mesh() const { return mesh_; }
        const render_context* get_context() const { return context_; }

    protected:
        primitive_type type_ = primitive_type::PT_NONE;
        mesh_base* mesh_ = nullptr;
        render_context* context_ = nullptr;
        render_args args_ = {};
    };
}}

#endif //ZAP_VISUAL_HPP
