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
        using mesh_base = zap::engine::mesh_base;
        using render_args = zap::renderer::render_args;
        using render_context = zap::renderer::render_context;

        visual() = default;
        visual(mesh_base* mesh, render_context* context) : mesh_{mesh}, context_{context}, args_{context} { }
        visual(const visual& rhs) : spatial_t(rhs), mesh_{rhs.mesh_}, context_{rhs.context_}, args_{rhs.context_} { }
        visual(visual&& rhs) noexcept : spatial_t(rhs), mesh_{rhs.mesh_}, context_{rhs.context_}, args_{rhs.context_} { }

        visual& operator=(const visual& rhs) {
            if(this != &rhs) {
                spatial_t::operator=(rhs);
                mesh_ = rhs.mesh_;
                context_ = rhs.context_;
                args_ = rhs.args_;
            }
            return *this;
        }

        visual& operator=(visual&& rhs) noexcept {
            if(this != &rhs) {
                spatial_t::operator=(std::move(rhs));
                mesh_ = rhs.mesh_;
                context_ = rhs.context_;
                args_ = std::move(rhs.args_);
            }
            return *this;
        }

        bool is_valid() const { return mesh_ != nullptr && context_ != nullptr; }

        void set_mesh(mesh_base* mesh) { mesh_ = mesh; }
        void set_context(render_context* context) { context_ = context; }
        mesh_base* get_mesh() { return mesh_; }
        render_context* get_context() { return context_; }
        const mesh_base* get_mesh() const { return mesh_; }
        const render_context* get_context() const { return context_; }

    protected:
        mesh_base* mesh_ = nullptr;
        render_context* context_ = nullptr;
        render_args args_ = {};
    };
}}

#endif //ZAP_VISUAL_HPP
