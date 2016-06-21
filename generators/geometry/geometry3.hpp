//
// Created by Darren Otgaar on 2016/06/19.
//

#ifndef ZAP_GEOMETRY3_HPP
#define ZAP_GEOMETRY3_HPP

#include <maths/vec2.hpp>
#include <maths/vec3.hpp>
#include <engine/engine.hpp>

namespace zap { namespace generators {
    using prim_t = zap::engine::primitive_type;
    using maths::vec3;

    template <typename VertexT, prim_t Primitive>
    class geometry3 {
    public:
        constexpr static prim_t primitive = Primitive;
        template <typename T> static std::vector<VertexT> make_cube(const vec3<T>& dimensions={1,1,1}) {
            static_assert(primitive == engine::primitive_type::PT_TRIANGLES, "make_cube expects PT_TRIANGLES");
            std::vector<VertexT> vertices(36);
            const auto normal = VertexT::find(engine::attribute_type::AT_NORMAL);
            const auto texcoord = VertexT::find(engine::attribute_type::AT_TEXCOORD1);
            const auto half_dim = T(.5)*dimensions;

            // Bottom Face [0,-1,0]
            vertices[ 0].position.set(-half_dim.x, -half_dim.y, -half_dim.z);
            if(texcoord != INVALID_IDX) vertices[ 0].set(texcoord, vec2<T>(0,0));
            vertices[ 1].position.set(+half_dim.x, -half_dim.y, -half_dim.z);
            if(texcoord != INVALID_IDX) vertices[ 1].set(texcoord, vec2<T>(1,0));
            vertices[ 2].position.set(+half_dim.x, -half_dim.y, +half_dim.z);
            if(texcoord != INVALID_IDX) vertices[ 2].set(texcoord, vec2<T>(1,1));
            vertices[ 3].position.set(-half_dim.x, -half_dim.y, -half_dim.z);
            if(texcoord != INVALID_IDX) vertices[ 3].set(texcoord, vec2<T>(0,0));
            vertices[ 4].position.set(+half_dim.x, -half_dim.y, +half_dim.z);
            if(texcoord != INVALID_IDX) vertices[ 4].set(texcoord, vec2<T>(1,1));
            vertices[ 5].position.set(-half_dim.x, -half_dim.y, +half_dim.z);
            if(texcoord != INVALID_IDX) vertices[ 5].set(texcoord, vec2<T>(0,1));
            if(normal != INVALID_IDX) for(int i = 0; i != 6; ++i) vertices[i].set(normal, vec3<T>(0,-1,0));
            if(texcoord != INVALID_IDX) {
                vertices[ 0].set(texcoord, vec2<T>(0,0));
                vertices[ 1].set(texcoord, vec2<T>(1,0));
                vertices[ 2].set(texcoord, vec2<T>(1,1));
                vertices[ 3].set(texcoord, vec2<T>(0,0));
                vertices[ 4].set(texcoord, vec2<T>(1,1));
                vertices[ 5].set(texcoord, vec2<T>(0,1));
            }

            // Front Face [0,0,1]
            vertices[ 6].position.set(-half_dim.x, -half_dim.y, +half_dim.z);
            vertices[ 7].position.set(+half_dim.x, -half_dim.y, +half_dim.z);
            vertices[ 8].position.set(+half_dim.x, +half_dim.y, +half_dim.z);
            vertices[ 9].position.set(-half_dim.x, -half_dim.y, +half_dim.z);
            vertices[10].position.set(+half_dim.x, +half_dim.y, +half_dim.z);
            vertices[11].position.set(-half_dim.x, +half_dim.y, +half_dim.z);
            if(normal != INVALID_IDX) for(int i = 6; i != 12; ++i) vertices[i].set(normal, vec3<T>(0,0,1));
            if(texcoord != INVALID_IDX) {
                vertices[ 6].set(texcoord, vec2<T>(0,0));
                vertices[ 7].set(texcoord, vec2<T>(1,0));
                vertices[ 8].set(texcoord, vec2<T>(1,1));
                vertices[ 9].set(texcoord, vec2<T>(0,0));
                vertices[10].set(texcoord, vec2<T>(1,1));
                vertices[11].set(texcoord, vec2<T>(0,1));
            }

            // Right Face [1,0,0]
            vertices[12].position.set(+half_dim.x, -half_dim.y, +half_dim.z);
            vertices[13].position.set(+half_dim.x, -half_dim.y, -half_dim.z);
            vertices[14].position.set(+half_dim.x, +half_dim.y, -half_dim.z);
            vertices[15].position.set(+half_dim.x, -half_dim.y, +half_dim.z);
            vertices[16].position.set(+half_dim.x, +half_dim.y, -half_dim.z);
            vertices[17].position.set(+half_dim.x, +half_dim.y, +half_dim.z);
            if(normal != INVALID_IDX) for(int i = 12; i != 18; ++i) vertices[i].set(normal, vec3<T>(1,0,0));
            if(texcoord != INVALID_IDX) {
                vertices[12].set(texcoord, vec2<T>(0,0));
                vertices[13].set(texcoord, vec2<T>(1,0));
                vertices[14].set(texcoord, vec2<T>(1,1));
                vertices[15].set(texcoord, vec2<T>(0,0));
                vertices[16].set(texcoord, vec2<T>(1,1));
                vertices[17].set(texcoord, vec2<T>(0,1));
            }

            // Back Face [0,0,-1]
            vertices[18].position.set(+half_dim.x, -half_dim.y, -half_dim.z);
            vertices[19].position.set(-half_dim.x, -half_dim.y, -half_dim.z);
            vertices[20].position.set(-half_dim.x, +half_dim.y, -half_dim.z);
            vertices[21].position.set(+half_dim.x, -half_dim.y, -half_dim.z);
            vertices[22].position.set(-half_dim.x, +half_dim.y, -half_dim.z);
            vertices[23].position.set(+half_dim.x, +half_dim.y, -half_dim.z);
            if(normal != INVALID_IDX) for(int i = 18; i != 24; ++i) vertices[i].set(normal, vec3<T>(0,0,-1));
            if(texcoord != INVALID_IDX) {
                vertices[18].set(texcoord, vec2<T>(0,0));
                vertices[19].set(texcoord, vec2<T>(1,0));
                vertices[20].set(texcoord, vec2<T>(1,1));
                vertices[21].set(texcoord, vec2<T>(0,0));
                vertices[22].set(texcoord, vec2<T>(1,1));
                vertices[23].set(texcoord, vec2<T>(0,1));
            }

            // Left Face [1,0,0]
            vertices[24].position.set(-half_dim.x, -half_dim.y, -half_dim.z);
            vertices[25].position.set(-half_dim.x, -half_dim.y, +half_dim.z);
            vertices[26].position.set(-half_dim.x, +half_dim.y, +half_dim.z);
            vertices[27].position.set(-half_dim.x, -half_dim.y, -half_dim.z);
            vertices[28].position.set(-half_dim.x, +half_dim.y, +half_dim.z);
            vertices[29].position.set(-half_dim.x, +half_dim.y, -half_dim.z);
            if(normal != INVALID_IDX) for(int i = 24; i != 30; ++i) vertices[i].set(normal, vec3<T>(-1,0,0));
            if(texcoord != INVALID_IDX) {
                vertices[24].set(texcoord, vec2<T>(0,0));
                vertices[25].set(texcoord, vec2<T>(1,0));
                vertices[26].set(texcoord, vec2<T>(1,1));
                vertices[27].set(texcoord, vec2<T>(0,0));
                vertices[28].set(texcoord, vec2<T>(1,1));
                vertices[29].set(texcoord, vec2<T>(0,1));
            }

            // Top Face [0,1,0]
            vertices[30].position.set(-half_dim.x, +half_dim.y, +half_dim.z);
            vertices[31].position.set(+half_dim.x, +half_dim.y, +half_dim.z);
            vertices[32].position.set(+half_dim.x, +half_dim.y, -half_dim.z);
            vertices[33].position.set(-half_dim.x, +half_dim.y, +half_dim.z);
            vertices[34].position.set(+half_dim.x, +half_dim.y, -half_dim.z);
            vertices[35].position.set(-half_dim.x, +half_dim.y, -half_dim.z);
            if(normal != INVALID_IDX) for(int i = 30; i != 36; ++i) vertices[i].set(normal, vec3<T>(0,1,0));
            if(texcoord != INVALID_IDX) {
                vertices[30].set(texcoord, vec2<T>(0,0));
                vertices[31].set(texcoord, vec2<T>(1,0));
                vertices[32].set(texcoord, vec2<T>(1,1));
                vertices[33].set(texcoord, vec2<T>(0,0));
                vertices[34].set(texcoord, vec2<T>(1,1));
                vertices[35].set(texcoord, vec2<T>(0,1));
            }

            return vertices;
        }
    };
}}

#endif //ZAP_GEOMETRY3_HPP
