//
// Created by Darren Otgaar on 2016/10/10.
//

#ifndef ZAP_VISUAL_HPP
#define ZAP_VISUAL_HPP

#include "spatial.hpp"
#include <engine/mesh.hpp>
#include <renderer/render_context.hpp>

namespace zap { namespace scene_graph {
    template<typename SpatialT>
    class visual : public SpatialT {
    public:
        using spatial_t = SpatialT;
        using mesh_base = zap::engine::mesh_base;
        using render_context = zap::renderer::render_context;

        visual() = default;
        visual(mesh_base* mesh, render_context* context) : mesh_{mesh}, context_(context) { }
        visual(const visual& rhs) : spatial_t(rhs), mesh_(rhs.mesh_), context_(rhs.context_) { }
        visual(visual&& rhs) noexcept : spatial_t(rhs), mesh_(rhs.mesh_), context_(rhs.context_) { }

        visual& operator=(const visual& rhs) {
            if(this != &rhs) {
                spatial_t::operator=(rhs);
                mesh_ = rhs.mesh_;
                context_ = rhs.context_;
            }
            return *this;
        }

        visual& operator=(visual&& rhs) noexcept {
            if(this != &rhs) {
                spatial_t::operator=(std::move(rhs));
                mesh_ = rhs.mesh_;
                context_ = rhs.context_;
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

        // TODO: Temporary, move to renderer
        void draw(zap::renderer::renderer& rndr) const {
            if(!mesh_ || !context_) return;
            mesh_->bind(); context_->bind(rndr);
            mesh_->draw();
            mesh_->release(); context_->release(rndr);
        }

    protected:
        mesh_base* mesh_ = nullptr;
        render_context* context_ = nullptr;
    };
}}

#endif //ZAP_VISUAL_HPP
