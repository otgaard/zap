//
// Created by otgaard on 2018/02/25.
//

#include "obj_loader.hpp"

namespace zap { namespace graphics {

obj_loader::mesh_p3n3t2_u32_ptr obj_loader::load(const std::string& path) {
    mesh_p3n3t2_u32_ptr mesh = std::make_unique<mesh_p3n3t2_u32_t>();
    return mesh;
}

}}