//
// Created by otgaard on 2018/02/25.
//

#ifndef ZAP_OBJ_LOADER_HPP
#define ZAP_OBJ_LOADER_HPP

#include <memory>
#include <string>
#include <vector>
#include <engine/mesh.hpp>

#include <tools/os.hpp>

#include "graphics/graphics3/g3_types.hpp"

// Loader for simple OBJ models for now, e.g. Stanford models, etc.
namespace zap { namespace graphics {
    class obj_loader {
    public:
        using mesh_p3n3t2_u32_t = zap::graphics::mesh_p3n3t2_u32_t;
        using mesh_p3n3t2_u32_ptr = std::unique_ptr<mesh_p3n3t2_u32_t>;

        static mesh_p3n3t2_u32_ptr load(const std::string& path);

    protected:

    private:

    };
}}


#endif //ZAP_OBJ_LOADER_HPP
