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
    using model_t = std::pair<std::vector<vtx_p3n3t2_t>, std::vector<uint32_t>>;

    template <typename VertexT, typename IndexT>
    static void compute_normals(std::vector<VertexT>& vbuf, std::vector<IndexT>& ibuf, bool clear=true) {
        // Computing Normals
        if(clear) for(auto& vtx : vbuf) vtx.normal.set(0.f, 0.f, 0.f);

        for(auto tri = 0; tri != ibuf.size(); tri += 3) {
            const auto A = ibuf[tri], B = ibuf[tri + 1], C = ibuf[tri + 2];
            const auto& pA = vbuf[A].position, &pB = vbuf[B].position, &pC = vbuf[C].position;
            const auto U = pB - pA, V = pC - pA, N = zap::maths::cross(U, V);
            vbuf[A].normal += N;
            vbuf[B].normal += N;
            vbuf[C].normal += N;
        }

        for(auto& vtx : vbuf) vtx.normal.normalise();
    }

    static std::string read_textfile(const std::string& path);

    static mesh_p3n3t2_u32_t load_mesh(const std::string& path);
    static model_t load_model(const std::string& path);

protected:

private:

};

}}

#endif //ZAP_OBJ_LOADER_HPP
