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
        visual(const visual& rhs) = delete;

        visual& operator=(const visual& rhs) = delete;

        bool is_valid() const { return mesh_ != nullptr && context_ != nullptr; }

        void set_mesh(mesh_base* mesh) { mesh_ = mesh; }
        void set_context(render_context* context) { context_ = context; }
        mesh_base* get_mesh() { return mesh_; }
        render_context* get_context() { return context_; }

        // TODO: Temporary interface, move to renderer
        void draw() const override {
            context_->bind();
            mesh_->bind();
            mesh_->draw();
            mesh_->release();
            context_->release();
        }

    protected:
        mesh_base* mesh_ = nullptr;
        render_context* context_ = nullptr;
    };
}}

#endif //ZAP_VISUAL_HPP
