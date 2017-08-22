//
// Created by Darren Otgaar on 2016/06/19.
//

#ifndef ZAP_GEOMETRY3_HPP
#define ZAP_GEOMETRY3_HPP

#include <map>
#include <graphics3/g3_types.hpp>
#include <generators/geometry/geometry_traits.hpp>

namespace zap { namespace generators {
    template <typename VertexT, primitive_type Primitive> struct geometry3;

    template <typename VertexT>
    struct geometry3<VertexT, primitive_type::PT_TRIANGLES> : public geometry_traits<VertexT, primitive_type::PT_TRIANGLES> {
        template <typename T> using vec2 = zap::maths::vec2<T>;
        template <typename T> using vec3 = zap::maths::vec3<T>;
        using vec2f = zap::maths::vec2f;
        using vec3f = zap::maths::vec3f;

        template <typename T>
        static std::tuple<std::vector<VertexT>, std::vector<uint16_t>> make_skybox(const vec3<T>& dims={T(1), T(1), T(1)});

        template <typename IndexT=uint16_t>
        static std::tuple<std::vector<VertexT>, std::vector<IndexT>>
        make_cylinder(int stacks, int slices, float height, float radius, bool inside=false);

        template <typename T>
        static std::vector<VertexT> make_cube(const vec3<T>& dimensions={T(1), T(1), T(1)});

        template <typename T, typename IndexT=uint16_t>
        static std::tuple<std::vector<VertexT>,std::vector<IndexT>> make_icosahedron(T size=1.f);

        template <typename IndexT=uint16_t>
        static std::tuple<std::vector<VertexT>, std::vector<IndexT>> make_icosphere(size_t subdivision_levels);

        template <typename T, typename IndexT=uint16_t>
        static std::tuple<std::vector<VertexT>, std::vector<IndexT>> make_UVsphere(int z_samples, int radial_samples, T radius, bool inside=false);
    };

    template <typename VertexT>
    template <typename T>
    std::tuple<std::vector<VertexT>, std::vector<uint16_t>>
    geometry3<VertexT, primitive_type::PT_TRIANGLES>::make_skybox(const vec3<T>& dims) {
        static_assert(std::is_signed<T>::value, "Skybox dimensions must be signed type");

        const auto hdims = dims/T(2);
        assert(hdims.x > T(0) && "Invalid dimensions set for skybox position");

        const auto normal = VertexT::find(engine::attribute_type::AT_NORMAL);

        std::vector<VertexT> vertices(8);
        std::vector<uint16_t> index{};

        // Bottom
        vertices[0].position.set(-hdims.x, -hdims.y, -hdims.z);
        vertices[0].set(normal, normalise(vertices[0].position));
        vertices[1].position.set(-hdims.x, -hdims.y, +hdims.z);
        vertices[1].set(normal, normalise(vertices[1].position));
        vertices[2].position.set(+hdims.x, -hdims.y, +hdims.z);
        vertices[2].set(normal, normalise(vertices[2].position));
        vertices[3].position.set(+hdims.x, -hdims.y, -hdims.z);
        vertices[3].set(normal, normalise(vertices[3].position));

        // Top
        vertices[4].position.set(-hdims.x, +hdims.y, -hdims.z);
        vertices[4].set(normal, normalise(vertices[4].position));
        vertices[5].position.set(-hdims.x, +hdims.y, +hdims.z);
        vertices[5].set(normal, normalise(vertices[5].position));
        vertices[6].position.set(+hdims.x, +hdims.y, +hdims.z);
        vertices[6].set(normal, normalise(vertices[6].position));
        vertices[7].position.set(+hdims.x, +hdims.y, -hdims.z);
        vertices[7].set(normal, normalise(vertices[7].position));

        // X+ X- Y+ Y- Z+ Z-
        index.emplace_back(3); index.emplace_back(2); index.emplace_back(6);        // +X
        index.emplace_back(3); index.emplace_back(6); index.emplace_back(7);

        index.emplace_back(1); index.emplace_back(0); index.emplace_back(4);        // -X
        index.emplace_back(1); index.emplace_back(4); index.emplace_back(5);

        index.emplace_back(4); index.emplace_back(7); index.emplace_back(6);        // +Y
        index.emplace_back(4); index.emplace_back(6); index.emplace_back(5);

        index.emplace_back(0); index.emplace_back(2); index.emplace_back(3);        // -Y
        index.emplace_back(0); index.emplace_back(1); index.emplace_back(2);

        index.emplace_back(1); index.emplace_back(6); index.emplace_back(2);        // +Z
        index.emplace_back(1); index.emplace_back(5); index.emplace_back(6);

        index.emplace_back(0); index.emplace_back(3); index.emplace_back(7);        // -Z
        index.emplace_back(0); index.emplace_back(7); index.emplace_back(4);

        return std::make_tuple(vertices, index);
    }

    template <typename VertexT>
    template <typename IndexT>
    std::tuple<std::vector<VertexT>, std::vector<IndexT>>
    geometry3<VertexT, primitive_type::PT_TRIANGLES>::make_cylinder(int stacks, int slices, float height, float radius, bool inside) {
        return std::make_tuple(std::vector<VertexT>{}, std::vector<IndexT>{});
    }

    template <typename VertexT>
    template <typename T>
    std::vector<VertexT>
    geometry3<VertexT, primitive_type::PT_TRIANGLES>::make_cube(const vec3<T>& dimensions) {
        std::vector<VertexT> vertices(36);
        const auto normal = VertexT::find(engine::attribute_type::AT_NORMAL);
        const auto texcoord = VertexT::find(engine::attribute_type::AT_TEXCOORD1);
        const auto half_dim = .5f*dimensions;

        // Bottom Face [0,-1,0]
        vertices[ 0].position.set(-half_dim.x, -half_dim.y, -half_dim.z);
        if(texcoord != INVALID_IDX) vertices[ 0].set(texcoord, vec2<T>{T(0), T(0)});
        vertices[ 1].position.set(+half_dim.x, -half_dim.y, -half_dim.z);
        if(texcoord != INVALID_IDX) vertices[ 1].set(texcoord, vec2<T>{T(1), T(0)});
        vertices[ 2].position.set(+half_dim.x, -half_dim.y, +half_dim.z);
        if(texcoord != INVALID_IDX) vertices[ 2].set(texcoord, vec2<T>{T(1), T(1)});
        vertices[ 3].position.set(-half_dim.x, -half_dim.y, -half_dim.z);
        if(texcoord != INVALID_IDX) vertices[ 3].set(texcoord, vec2<T>{T(0), T(0)});
        vertices[ 4].position.set(+half_dim.x, -half_dim.y, +half_dim.z);
        if(texcoord != INVALID_IDX) vertices[ 4].set(texcoord, vec2<T>{T(1), T(1)});
        vertices[ 5].position.set(-half_dim.x, -half_dim.y, +half_dim.z);
        if(texcoord != INVALID_IDX) vertices[ 5].set(texcoord, vec2<T>{T(0), T(1)});
        if(normal != INVALID_IDX) for(int i = 0; i != 6; ++i) vertices[i].set(normal, vec3<T>{T(0), -T(1), T(0)});
        if(texcoord != INVALID_IDX) {
            vertices[ 0].set(texcoord, vec2<T>{T(0), T(0)});
            vertices[ 1].set(texcoord, vec2<T>{T(1), T(0)});
            vertices[ 2].set(texcoord, vec2<T>{T(1), T(1)});
            vertices[ 3].set(texcoord, vec2<T>{T(0), T(0)});
            vertices[ 4].set(texcoord, vec2<T>{T(1), T(1)});
            vertices[ 5].set(texcoord, vec2<T>{T(0), T(1)});
        }

        // Front Face [0,0,1]
        vertices[ 6].position.set(-half_dim.x, -half_dim.y, +half_dim.z);
        vertices[ 7].position.set(+half_dim.x, -half_dim.y, +half_dim.z);
        vertices[ 8].position.set(+half_dim.x, +half_dim.y, +half_dim.z);
        vertices[ 9].position.set(-half_dim.x, -half_dim.y, +half_dim.z);
        vertices[10].position.set(+half_dim.x, +half_dim.y, +half_dim.z);
        vertices[11].position.set(-half_dim.x, +half_dim.y, +half_dim.z);
        if(normal != INVALID_IDX) for(int i = 6; i != 12; ++i) vertices[i].set(normal, vec3<T>{T(0), T(0), T(1)});
        if(texcoord != INVALID_IDX) {
            vertices[ 6].set(texcoord, vec2<T>{T(0), T(0)});
            vertices[ 7].set(texcoord, vec2<T>{T(1), T(0)});
            vertices[ 8].set(texcoord, vec2<T>{T(1), T(1)});
            vertices[ 9].set(texcoord, vec2<T>{T(0), T(0)});
            vertices[10].set(texcoord, vec2<T>{T(1), T(1)});
            vertices[11].set(texcoord, vec2<T>{T(0), T(1)});
        }

        // Right Face [1,0,0]
        vertices[12].position.set(+half_dim.x, -half_dim.y, +half_dim.z);
        vertices[13].position.set(+half_dim.x, -half_dim.y, -half_dim.z);
        vertices[14].position.set(+half_dim.x, +half_dim.y, -half_dim.z);
        vertices[15].position.set(+half_dim.x, -half_dim.y, +half_dim.z);
        vertices[16].position.set(+half_dim.x, +half_dim.y, -half_dim.z);
        vertices[17].position.set(+half_dim.x, +half_dim.y, +half_dim.z);
        if(normal != INVALID_IDX) for(int i = 12; i != 18; ++i) vertices[i].set(normal, vec3<T>{T(1), T(0), T(0)});
        if(texcoord != INVALID_IDX) {
            vertices[12].set(texcoord, vec2<T>{T(0), T(0)});
            vertices[13].set(texcoord, vec2<T>{T(1), T(0)});
            vertices[14].set(texcoord, vec2<T>{T(1), T(1)});
            vertices[15].set(texcoord, vec2<T>{T(0), T(0)});
            vertices[16].set(texcoord, vec2<T>{T(1), T(1)});
            vertices[17].set(texcoord, vec2<T>{T(0), T(1)});
        }

        // Back Face [0,0,-1]
        vertices[18].position.set(+half_dim.x, -half_dim.y, -half_dim.z);
        vertices[19].position.set(-half_dim.x, -half_dim.y, -half_dim.z);
        vertices[20].position.set(-half_dim.x, +half_dim.y, -half_dim.z);
        vertices[21].position.set(+half_dim.x, -half_dim.y, -half_dim.z);
        vertices[22].position.set(-half_dim.x, +half_dim.y, -half_dim.z);
        vertices[23].position.set(+half_dim.x, +half_dim.y, -half_dim.z);
        if(normal != INVALID_IDX) for(int i = 18; i != 24; ++i) vertices[i].set(normal, vec3<T>{T(0), T(0), -T(1)});
        if(texcoord != INVALID_IDX) {
            vertices[18].set(texcoord, vec2<T>{T(0), T(0)});
            vertices[19].set(texcoord, vec2<T>{T(1), T(0)});
            vertices[20].set(texcoord, vec2<T>{T(1), T(1)});
            vertices[21].set(texcoord, vec2<T>{T(0), T(0)});
            vertices[22].set(texcoord, vec2<T>{T(1), T(1)});
            vertices[23].set(texcoord, vec2<T>{T(0), T(1)});
        }

        // Left Face [1,0,0]
        vertices[24].position.set(-half_dim.x, -half_dim.y, -half_dim.z);
        vertices[25].position.set(-half_dim.x, -half_dim.y, +half_dim.z);
        vertices[26].position.set(-half_dim.x, +half_dim.y, +half_dim.z);
        vertices[27].position.set(-half_dim.x, -half_dim.y, -half_dim.z);
        vertices[28].position.set(-half_dim.x, +half_dim.y, +half_dim.z);
        vertices[29].position.set(-half_dim.x, +half_dim.y, -half_dim.z);
        if(normal != INVALID_IDX) for(int i = 24; i != 30; ++i) vertices[i].set(normal, vec3<T>{-T(1), T(0), T(0)});
        if(texcoord != INVALID_IDX) {
            vertices[24].set(texcoord, vec2<T>{T(0), T(0)});
            vertices[25].set(texcoord, vec2<T>{T(1), T(0)});
            vertices[26].set(texcoord, vec2<T>{T(1), T(1)});
            vertices[27].set(texcoord, vec2<T>{T(0), T(0)});
            vertices[28].set(texcoord, vec2<T>{T(1), T(1)});
            vertices[29].set(texcoord, vec2<T>{T(0), T(1)});
        }

        // Top Face [0,1,0]
        vertices[30].position.set(-half_dim.x, +half_dim.y, +half_dim.z);
        vertices[31].position.set(+half_dim.x, +half_dim.y, +half_dim.z);
        vertices[32].position.set(+half_dim.x, +half_dim.y, -half_dim.z);
        vertices[33].position.set(-half_dim.x, +half_dim.y, +half_dim.z);
        vertices[34].position.set(+half_dim.x, +half_dim.y, -half_dim.z);
        vertices[35].position.set(-half_dim.x, +half_dim.y, -half_dim.z);
        if(normal != INVALID_IDX) for(int i = 30; i != 36; ++i) vertices[i].set(normal, vec3<T>{T(0), T(1), T(0)});
        if(texcoord != INVALID_IDX) {
            vertices[30].set(texcoord, vec2<T>{T(0), T(0)});
            vertices[31].set(texcoord, vec2<T>{T(1), T(0)});
            vertices[32].set(texcoord, vec2<T>{T(1), T(1)});
            vertices[33].set(texcoord, vec2<T>{T(0), T(0)});
            vertices[34].set(texcoord, vec2<T>{T(1), T(1)});
            vertices[35].set(texcoord, vec2<T>{T(0), T(1)});
        }

        return vertices;
    }

    template <typename VertexT>
    template <typename T, typename IndexT>
    std::tuple<std::vector<VertexT>, std::vector<IndexT>>
    geometry3<VertexT, primitive_type::PT_TRIANGLES>::make_icosahedron(T size) {
        const float t = .5f*(1.f + std::sqrt(5.f));
        const vec2<T> N = maths::normalise(vec2<T>(t,1));

        std::vector<VertexT> vertices(12);

        // plane 1
        vertices[0].position.set(-N.y, +N.x, 0);
        vertices[1].position.set(+N.y, +N.x, 0);
        vertices[2].position.set(-N.y, -N.x, 0);
        vertices[3].position.set(+N.y, -N.x, 0);

        // plane 2
        vertices[4].position.set(0, -N.y, +N.x);
        vertices[5].position.set(0, +N.y, +N.x);
        vertices[6].position.set(0, -N.y, -N.x);
        vertices[7].position.set(0, +N.y, -N.x);

        // plane 3
        vertices[8].position.set(+N.x, 0, -N.y);
        vertices[9].position.set(+N.x, 0, +N.y);
        vertices[10].position.set(-N.x, 0, -N.y);
        vertices[11].position.set(-N.x, 0, +N.y);

        std::vector<IndexT> indices = { 0, 11, 5, 0, 5, 1, 0, 1, 7, 0, 7, 10, 0, 10, 11, 1, 5, 9, 5, 11, 4,
                                        11, 10, 2, 10, 7, 6, 7, 1, 8, 3, 9, 4, 3, 4, 2, 3, 2, 6, 3, 6, 8, 3,
                                        8, 9, 4, 9, 5, 2, 4, 11, 6, 2, 10, 8, 6, 7, 9, 8, 1 };

        return std::make_tuple(vertices, indices);
    }

    template <typename VertexT>
    template <typename IndexT>
    std::tuple<std::vector<VertexT>, std::vector<IndexT>>
    geometry3<VertexT, primitive_type::PT_TRIANGLES>::make_icosphere(size_t subdivision_levels) {
        if(subdivision_levels > 10) return std::make_tuple(std::vector<VertexT>(), std::vector<IndexT>());
        const size_t vertex_exp[10] = { 58, 263, 1104, 4476, 18002, 72135, 282864, 889383, 2995613, 10982403 };
        //const size_t index_exp[10] = { 240, 960, 3840, 15360, 61440, 245760, 983040, 3932160, 15728640, 62914560 };

        size_t vertex_count = std::accumulate(vertex_exp, vertex_exp+subdivision_levels, size_t(0));
        //size_t index_count = std::accumulate(index_exp, index_exp+subdivision_levels, size_t(0));

        using edge_midpoint_tbl = std::map<std::tuple<int,int>, int>;
        std::vector<VertexT> vbuf;
        std::vector<IndexT> ibuf;

        std::tie(vbuf, ibuf) = generators::geometry3<VertexT, primitive_type::PT_TRIANGLES>::template make_icosahedron<float>();

        vbuf.reserve(vertex_count);   // TODO: Work out formula to calculate number of vertices & faces

        edge_midpoint_tbl midpoints;

        auto get_centre = [&vbuf, &midpoints](int p1, int p2)->int {
            auto key = p1 < p2 ? std::make_tuple(p1,p2) : std::make_tuple(p2,p1);
            auto it = midpoints.find(key);
            if(it != midpoints.end()) return it->second;

            auto& pnt1 = vbuf[p1].position;
            auto& pnt2 = vbuf[p2].position;

            int idx = int(vbuf.size());
            vbuf.push_back({{normalise(.5f*(pnt1+pnt2))}});
            midpoints.insert(std::make_pair(std::make_tuple(p1,p2), idx));
            return idx;
        };

        for(size_t sub = 0; sub != subdivision_levels; ++sub) {
            std::vector<uint16_t> new_ibuf;
            new_ibuf.reserve(ibuf.size()*4*3);

            for(size_t idx = 0, iend = ibuf.size()/3; idx != iend; ++idx) {
                int offset = 3*idx, oA = ibuf[offset], oB = ibuf[offset+1], oC = ibuf[offset+2];
                int nA = get_centre(oA,oB), nB = get_centre(oB,oC), nC = get_centre(oC,oA);
                new_ibuf.push_back(oA); new_ibuf.push_back(nA), new_ibuf.push_back(nC);
                new_ibuf.push_back(oB); new_ibuf.push_back(nB), new_ibuf.push_back(nA);
                new_ibuf.push_back(oC); new_ibuf.push_back(nC), new_ibuf.push_back(nB);
                new_ibuf.push_back(nA); new_ibuf.push_back(nB), new_ibuf.push_back(nC);
            }

            ibuf = std::move(new_ibuf);
        }

        return std::make_tuple(vbuf, ibuf);
    }

    template <typename VertexT>
    template <typename T, typename IndexT>
    std::tuple<std::vector<VertexT>, std::vector<IndexT>>
    geometry3<VertexT, primitive_type::PT_TRIANGLES>::make_UVsphere(int z_samples, int radial_samples, T radius, bool inside) {
        int zsm1 = z_samples-1, zsm2 = z_samples-2, zsm3 = z_samples-3;
        int rsp1 = radial_samples+1;
        int vertex_count = zsm2*rsp1 + 2;
        int tri_count = 2*zsm2*radial_samples;
        int index_count = 3*tri_count;

        const auto normal_idx = VertexT::find(engine::attribute_type::AT_NORMAL);
        const auto texcoord_idx = VertexT::find(engine::attribute_type::AT_TEXCOORD1);
        auto vbuf = std::vector<VertexT>(vertex_count);

        T invRS = T(1)/radial_samples;
        T zFactor = T(2)/zsm1;
        int r, z, zStart, i;
        vec2<T> tcoord;

        std::vector<T> sn(rsp1);
        std::vector<T> cs(rsp1);

        for(r = 0; r < radial_samples; ++r) {
            T angle = maths::TWO_PI<T>*invRS*r;
            cs[r] = std::cos(angle); sn[r] = std::sin(angle);
        }

        sn[radial_samples] = sn[0];
        cs[radial_samples] = cs[0];

        for(z = 1, i = 0; z < zsm1; ++z) {
            T zFraction = -1.0f + zFactor*z;
            T zValue = radius*zFraction;

            vec3<T> sliceCenter(0, 0, zValue);

            T sliceRadius = std::sqrt(std::abs(radius*radius - zValue*zValue));

            vec3<T> normal;
            int save = i;
            for(r = 0; r < radial_samples; ++r) {
                T radial_fraction = r*invRS;
                vec3<T> radial(cs[r], sn[r], 0.0f);

                vbuf[i].position = sliceCenter + sliceRadius*radial;

                if(normal_idx != INVALID_IDX) {
                    normal = vbuf[i].position;
                    normal.normalise();
                    vbuf[i].set(normal_idx, inside ? -normal : normal);
                }

                if(texcoord_idx != INVALID_IDX) {
                    tcoord[0] = radial_fraction;
                    tcoord[1] = T(0.5) * (zFraction + T(1));
                    vbuf[i].set(texcoord_idx, tcoord);
                }

                ++i;
            }

            vbuf[i].position = vbuf[save].position;
            if(normal_idx != INVALID_IDX) vbuf[i].set(normal_idx, *vbuf[save].template get<vec3<T>>(normal_idx));

            if(texcoord_idx != INVALID_IDX) {
                tcoord[0] = T(1);
                tcoord[1] = T(0.5) * (zFraction + T(1));
                vbuf[i].set(texcoord_idx, tcoord);
            }

            ++i;
        }

        vbuf[i].position = vec3<T>(0, 0, -radius);
        if(normal_idx != INVALID_IDX) vbuf[i].set(normal_idx, inside ? vec3<T>(0, 0, T(1)) : vec3<T>(0, 0, T(-1)));

        if(texcoord_idx != INVALID_IDX) {
            tcoord = vec2<T>(T(0.5), T(0));
            vbuf[i].set(texcoord_idx, tcoord);
        }

        ++i;

        vbuf[i].position = vec3<T>(0, 0, radius);
        if(normal_idx != INVALID_IDX) vbuf[i].set(normal_idx, inside ? vec3<T>(0, 0, T(-1)) : vec3<T>(0, 0, T(1)));

        if(texcoord_idx != INVALID_IDX) {
            tcoord = vec2<T>(T(0.5), T(1));
            vbuf[i].set(texcoord_idx, tcoord);
        }

        std::vector<IndexT> ibuf(index_count);
        auto* indices = ibuf.data();

        for(z = 0, zStart = 0; z < zsm3; ++z) {
            int i0 = zStart, i1 = i0 + 1;
            zStart += rsp1;
            int i2 = zStart, i3 = i2 + 1;
            for(i = 0; i < radial_samples; ++i, indices += 6) {
                if(inside) {
                    indices[0] = i0++; indices[1] = i2;   indices[2] = i1;
                    indices[3] = i1++; indices[4] = i2++; indices[5] = i3++;
                } else {
                    indices[0] = i0++; indices[1] = i1;   indices[2] = i2;
                    indices[3] = i1++; indices[4] = i3++; indices[5] = i2++;
                }
            }
        }

        int numVerticesM2 = vertex_count - 2;
        for(i = 0; i < radial_samples; ++i, indices += 3) {
            if(inside) {
                indices[0] = i; indices[1] = i + 1; indices[2] = numVerticesM2;
            } else {
                indices[0] = i; indices[1] = numVerticesM2; indices[2] = i + 1;
            }
        }

        int numVerticesM1 = vertex_count-1, offset = zsm3*rsp1;
        for(i = 0; i < radial_samples; ++i, indices += 3) {
            if(inside) {
                indices[0] = i + offset; indices[1] = numVerticesM1; indices[2] = i + 1 + offset;
            } else {
                indices[0] = i + offset; indices[1] = i + 1 + offset; indices[2] = numVerticesM1;
            }
        }

        return std::make_tuple(vbuf, ibuf);
    }

    ////////////////////////////////////////////
    // PT_TRIANGLE_STRIP

    template <typename VertexT>
    struct geometry3<VertexT, primitive_type::PT_TRIANGLE_STRIP> : public geometry_traits<VertexT, primitive_type::PT_TRIANGLE_STRIP> {
        template <typename IndexT=uint16_t>
        static std::tuple<std::vector<VertexT>, std::vector<IndexT>>
        make_cylinder(int stacks, int slices, float height, float radius, bool inside=false);
    };

    // PT_TRIANGLE_STRIP + Index Implementation
    template <typename VertexT>
    template <typename IndexT>
    std::tuple<std::vector<VertexT>, std::vector<IndexT>>
    geometry3<VertexT, primitive_type::PT_TRIANGLE_STRIP>::make_cylinder(int stacks, int slices, float height, float radius, bool inside) {
        const auto dt = maths::TWO_PI<float>/slices;
        const auto dh = height/stacks;
        const auto hh = height/2;

        const auto slicesp1 = slices+1, stacksp1 = stacks+1;
        const auto vertex_count = slicesp1 * stacksp1;
        const auto index_count = stacks*(2*slicesp1 + 1); // Primitive Restart

        assert(index_count <= std::numeric_limits<IndexT>::max() && "Index type too small for mesh");

        std::vector<VertexT> vertices(vertex_count);
        std::vector<uint16_t> indices(index_count);

        // Calculate the first coords once & copy
        for(int t = 0; t != slicesp1; ++t) {
            const auto theta = (t%slices)*dt, ctheta = std::cos(theta), stheta = std::sin(theta);
            vertices[t].position.set(radius*ctheta, radius*stheta, hh);
            inside ? vertices[t].normal.set(-ctheta, -stheta, 0) : vertices[t].normal.set(ctheta, stheta, 0);
            vertices[t].texcoord1.set(t/float(slices), 0.f);
        }

        for(int h = 1, h_end = stacksp1; h != h_end; ++h) {
            const auto v_offset = h*slicesp1;
            const auto i_offset = (h-1)*(2*slicesp1 + 1);
            const auto hgt = hh - h*dh;
            const auto v = h/float(stacks);
            std::copy(vertices.begin(), vertices.begin()+slicesp1, vertices.begin()+v_offset);
            for(int t = 0; t != slicesp1; ++t) {
                const auto idx = v_offset+t;
                vertices[idx].position.z = hgt;
                vertices[idx].texcoord1.y = v;
                if(inside) {
                    indices[i_offset + 2 * t] = IndexT(idx);
                    indices[i_offset + 2 * t + 1] = IndexT(idx - slicesp1);
                } else {
                    indices[i_offset + 2 * t] = IndexT(idx - slicesp1);
                    indices[i_offset + 2 * t + 1] = IndexT(idx);
                }
            }

            indices[i_offset+2*slicesp1] = std::numeric_limits<uint16_t>::max();
        }

        return std::make_tuple(vertices, indices);
    }

    ////////////////////////////////////////////
    // PT_TRIANGLE_FAN

    template <typename VertexT>
    struct geometry3<VertexT, primitive_type::PT_TRIANGLE_FAN> : public geometry_traits<VertexT, primitive_type::PT_TRIANGLE_FAN> {
        template <typename T> using vec2 = zap::maths::vec2<T>;
        template <typename T> using vec3 = zap::maths::vec3<T>;
        using vec2f = zap::maths::vec2f;
        using vec3f = zap::maths::vec3f;

        static std::vector<VertexT>
        make_quad(float width, float height);
    };

    template <typename VertexT>
    std::vector<VertexT> geometry3<VertexT, primitive_type::PT_TRIANGLE_FAN>::make_quad(float width, float height) {
        const auto hwidth = width/2.f, hheight = height/2.f;
        const auto normal = vec3f{0.f, 0.f, 1.f};
        const auto normal_idx = VertexT::find(engine::attribute_type::AT_NORMAL);
        const auto texcoord_idx = VertexT::find(engine::attribute_type::AT_TEXCOORD1);
        std::vector<VertexT> quad(4);

        quad[0].position.set(-hwidth, -hheight, 0.f);
        if(normal_idx != INVALID_IDX) quad[0].set(normal_idx, normal);
        if(texcoord_idx != INVALID_IDX) quad[0].set(texcoord_idx, vec2f{0.f, 0.f});

        quad[1].position.set(+hwidth, -hheight, 0.f);
        if(normal_idx != INVALID_IDX) quad[1].set(normal_idx, normal);
        if(texcoord_idx != INVALID_IDX) quad[1].set(texcoord_idx, vec2f{1.f, 0.f});

        quad[2].position.set(+hwidth, +hheight, 0.f);
        if(normal_idx != INVALID_IDX) quad[2].set(normal_idx, normal);
        if(texcoord_idx != INVALID_IDX) quad[2].set(texcoord_idx, vec2f{1.f, 1.f});

        quad[3].position.set(-hwidth, +hheight, 0.f);
        if(normal_idx != INVALID_IDX) quad[3].set(normal_idx, normal);
        if(texcoord_idx != INVALID_IDX) quad[3].set(texcoord_idx, vec2f{0.f, 1.f});

        return quad;
    }
}}

#endif //ZAP_GEOMETRY3_HPP
