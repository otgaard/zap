//
// Created by Darren Otgaar on 2016/10/10.
//

#ifndef ZAP_VISUAL_HPP
#define ZAP_VISUAL_HPP

#include "spatial.hpp"
#include <engine/mesh.hpp>
#include <engine/program.hpp>

namespace zap { namespace scene_graph {
    template<typename SpatialT>
    class visual {
    public:
        using spatial_t = SpatialT;
        using mesh_base = zap::engine::mesh_base;
        using program = zap::engine::program;

        visual() = default;
        visual(mesh_base* mesh, program* program) : mesh_{mesh}, prog_{program} { }
        visual(const visual& rhs) = delete;

        void set_mesh(mesh_base* mesh) { mesh_ = mesh; }
        void set_program(program* prog) { prog_ = prog; }
        bool is_valid() const { return mesh_ != nullptr && prog_ != nullptr; }

        const mesh_base* get_mesh() const { return mesh_; }
        const program* get_program() const { return prog_; }
        mesh_base* get_mesh() { return mesh_; }
        program* get_program() { return prog_; }

    protected:
        mesh_base* mesh_ = nullptr;
        program* prog_ = nullptr;
    };
}}

#endif //ZAP_VISUAL_HPP
