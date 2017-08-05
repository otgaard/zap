//
// Created by Darren Otgaar on 2016/10/10.
//

#ifndef ZAP_VISUAL_HPP
#define ZAP_VISUAL_HPP

#include "spatial.hpp"
#include <engine/mesh.hpp>
#include <engine/program.hpp>
#include <renderer/render_context.hpp>

namespace zap { namespace scene_graph {
    template<typename SpatialT>
    class visual : public SpatialT {
    public:
        using spatial_t = SpatialT;
        using mesh_base = zap::engine::mesh_base;
        using program = zap::engine::program;
        using render_context = zap::renderer::render_context;

        visual() = default;
        visual(mesh_base* mesh, program* program) : mesh_{mesh}, prog_{program} { }
        visual(mesh_base* mesh, render_context* context) : mesh_{mesh}, context_(context) { }
        visual(const visual& rhs) = delete;

        void set_mesh(mesh_base* mesh) { mesh_ = mesh; }
        void set_program(program* prog) { prog_ = prog; }
        void set_context(render_context* context) { context_ = context; }
        bool is_valid() const { return mesh_ != nullptr && prog_ != nullptr; }

        const mesh_base* get_mesh() const { return mesh_; }
        const program* get_program() const { return prog_; }
        const render_context* get_context() const { return context_; }
        mesh_base* get_mesh() { return mesh_; }
        program* get_program() { return prog_; }

    protected:
        mesh_base* mesh_ = nullptr;
        program* prog_ = nullptr;
        render_context* context_ = nullptr;
    };
}}

#endif //ZAP_VISUAL_HPP
