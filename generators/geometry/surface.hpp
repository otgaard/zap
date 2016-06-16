/* Created by Darren Otgaar on 2016/05/27. http://www.github.com/otgaard/zap */
#ifndef ZAP_SURFACE_HPP
#define ZAP_SURFACE_HPP

// Surface extraction algorithms
#include <maths/vec2.hpp>
#include <maths/vec3.hpp>
#include <engine/vertex_buffer.hpp>

namespace zap { namespace generators {
    struct surface_defs {
        using vec2i = zap::maths::vec2i;
        using vec3f = zap::maths::vec3f;

        static const vec3f vertex_offset_tbl[8];                // Vertex offsets from [0,0,0]
        static const vec2i edge_connection_tbl[12];             // Edge endpoints in cube
        static const vec3f edge_direction_tbl[12];              // Edge direction vectors
        static const int cube_edge_flag_tbl[256];               // Lists edge intersection for each possible config
        static const int triangle_connection_tbl[256][16];      // Lists the triangle config for each cube

        static const int tetra_edge_connection_tbl[6][2];
        static const int tetra_cube_tbl[6][4];
        static const int tetra_edge_flag_tbl[16];
        static const int tetra_triangle_tbl[16][7];
    };

    template <typename VBuffer>
    class surface {
    public:
        template<typename T> using vec3 = zap::maths::vec3<T>;

        static constexpr float fTargetValue = 70.f;
        static constexpr int iDataSetSize = 30;
        static constexpr float fStepSize = 1.f/iDataSetSize;

        using vertex_t = typename VBuffer::vertex_t;
        using buffer_t = std::vector<vertex_t>;

        template <typename Isosurface>
        static void marching_cubes(Isosurface fnc, buffer_t& buffer) {
            int x, y, z;
            for(x = 0; x < iDataSetSize; ++x)
                for(y = 0; y < iDataSetSize; ++y)
                    for(z = 0; z < iDataSetSize; ++z)
                        marching_cubes(fnc, buffer, x*fStepSize, y*fStepSize, z*fStepSize, fStepSize);
        }

        template <typename Isosurface>
        static void marching_tetrahedron(Isosurface fnc, buffer_t& buffer) {
            int x, y, z;
            for(x = 0; x < iDataSetSize; ++x)
                for(y = 0; y < iDataSetSize; ++y)
                    for(z = 0; z < iDataSetSize; ++z)
                        marching_tetrahedron(fnc, buffer, x*fStepSize, y*fStepSize, z*fStepSize, fStepSize);
        }


        template <typename Isosurface>
        static void marching_cubes(Isosurface fnc, buffer_t& buffer, float x, float y, float z, float scale) {
            using zap::maths::vec3f;

            static const auto& vertex_offset_tbl = surface_defs::vertex_offset_tbl;
            static const auto& edge_connection_tbl = surface_defs::edge_connection_tbl;
            static const auto& edge_direction_tbl = surface_defs::edge_direction_tbl;
            static const auto& cube_edge_flag_tbl = surface_defs::cube_edge_flag_tbl;
            static const auto& triangle_connection_tbl = surface_defs::triangle_connection_tbl;

            int iCorner, iVertex, iVertexTest, iEdge, iTriangle, iFlagIndex, iEdgeFlags;
            float fOffset;
            vec3f sColor;
            float afCubeValue[8];
            vec3f asEdgeVertex[12];
            vec3f asEdgeNorm[12];

            //Make a local copy of the values at the cube's corners
            for(iVertex = 0; iVertex < 8; iVertex++) {
                afCubeValue[iVertex] = fnc(x + vertex_offset_tbl[iVertex][0]*scale,
                                           y + vertex_offset_tbl[iVertex][1]*scale,
                                           z + vertex_offset_tbl[iVertex][2]*scale);
            }

            //Find which vertices are inside of the surface and which are outside
            iFlagIndex = 0;
            for(iVertexTest = 0; iVertexTest < 8; iVertexTest++) {
                if(afCubeValue[iVertexTest] <= fTargetValue)
                    iFlagIndex |= 1<<iVertexTest;
            }

            //Find which edges are intersected by the surface
            iEdgeFlags = cube_edge_flag_tbl[iFlagIndex];

            //If the cube is entirely inside or outside of the surface, then there will be no intersections
            if(iEdgeFlags == 0) return;

            //Find the point of intersection of the surface with each edge
            //Then find the normal to the surface at those points
            for(iEdge = 0; iEdge < 12; iEdge++) {
                //if there is an intersection on this edge
                if(iEdgeFlags & (1<<iEdge)) {
                    fOffset = fGetOffset(afCubeValue[ edge_connection_tbl[iEdge][0] ],
                                         afCubeValue[ edge_connection_tbl[iEdge][1] ], fTargetValue);

                    asEdgeVertex[iEdge].x = x + (vertex_offset_tbl[ edge_connection_tbl[iEdge][0] ][0]  +  fOffset * edge_direction_tbl[iEdge][0]) * scale;
                    asEdgeVertex[iEdge].y = y + (vertex_offset_tbl[ edge_connection_tbl[iEdge][0] ][1]  +  fOffset * edge_direction_tbl[iEdge][1]) * scale;
                    asEdgeVertex[iEdge].z = z + (vertex_offset_tbl[ edge_connection_tbl[iEdge][0] ][2]  +  fOffset * edge_direction_tbl[iEdge][2]) * scale;

                    normal(fnc, asEdgeVertex[iEdge].x, asEdgeVertex[iEdge].y, asEdgeVertex[iEdge].z, asEdgeNorm[iEdge]);
                }
            }

            //Draw the triangles that were found.  There can be up to five per cube
            for(iTriangle = 0; iTriangle < 5; iTriangle++) {
                if(triangle_connection_tbl[iFlagIndex][3*iTriangle] < 0) break;

                for(iCorner = 0; iCorner < 3; iCorner++) {
                    iVertex = triangle_connection_tbl[iFlagIndex][3*iTriangle+iCorner];

                    vertex_t vtx;
                    //vGetColor(vtx.colour1, asEdgeNorm[iVertex]);
                    vtx.normal = asEdgeNorm[iVertex];
                    vtx.position = asEdgeVertex[iVertex];
                    //vtx.texcoord1 = maths::vec2f(vtx.position.x, vtx.position.y);
                    buffer.push_back(vtx);
                }
            }
        }

        template <typename Isosurface>
        static void marching_tetrahedron(Isosurface fnc, buffer_t& buffer, vec3<float>* pasTetrahedronPosition, float* pafTetrahedronValue) {
            static const auto& tetra_edge_connection_tbl = surface_defs::tetra_edge_connection_tbl;
            static const auto& tetra_edge_flag_tbl = surface_defs::tetra_edge_flag_tbl;
            static const auto& tetra_triangle_tbl = surface_defs::tetra_triangle_tbl;
            using zap::maths::vec3f;

            int iEdge, iVert0, iVert1, iEdgeFlags, iTriangle, iCorner, iVertex, iFlagIndex = 0;
            float fOffset, fInvOffset = 0.0;
            vec3f asEdgeVertex[6];
            vec3f asEdgeNorm[6];
            vec3f sColor;

            //Find which vertices are inside of the surface and which are outside
            for(iVertex = 0; iVertex < 4; iVertex++) {
                if(pafTetrahedronValue[iVertex] <= fTargetValue)
                    iFlagIndex |= 1<<iVertex;
            }

            //Find which edges are intersected by the surface
            iEdgeFlags = tetra_edge_flag_tbl[iFlagIndex];

            //If the tetrahedron is entirely inside or outside of the surface, then there will be no intersections
            if(iEdgeFlags == 0) return;

            //Find the point of intersection of the surface with each edge
            // Then find the normal to the surface at those points
            for(iEdge = 0; iEdge < 6; iEdge++) {
                //if there is an intersection on this edge
                if(iEdgeFlags & (1<<iEdge)) {
                    iVert0 = tetra_edge_connection_tbl[iEdge][0];
                    iVert1 = tetra_edge_connection_tbl[iEdge][1];
                    fOffset = fGetOffset(pafTetrahedronValue[iVert0], pafTetrahedronValue[iVert1], fTargetValue);
                    fInvOffset = 1.0 - fOffset;

                    asEdgeVertex[iEdge].x = fInvOffset*pasTetrahedronPosition[iVert0].x  +  fOffset*pasTetrahedronPosition[iVert1].x;
                    asEdgeVertex[iEdge].y = fInvOffset*pasTetrahedronPosition[iVert0].y  +  fOffset*pasTetrahedronPosition[iVert1].y;
                    asEdgeVertex[iEdge].z = fInvOffset*pasTetrahedronPosition[iVert0].z  +  fOffset*pasTetrahedronPosition[iVert1].z;

                    normal(fnc, asEdgeVertex[iEdge].x, asEdgeVertex[iEdge].y, asEdgeVertex[iEdge].z,  asEdgeNorm[iEdge]);
                }
            }
            //Draw the triangles that were found.  There can be up to 2 per tetrahedron
            for(iTriangle = 0; iTriangle < 2; iTriangle++) {
                if(tetra_triangle_tbl[iFlagIndex][3*iTriangle] < 0) break;

                for(iCorner = 0; iCorner < 3; iCorner++) {
                    iVertex = tetra_triangle_tbl[iFlagIndex][3*iTriangle+iCorner];
                    vertex_t vtx;
                    //vGetColor(vtx.colour1, asEdgeNorm[iVertex]);
                    vtx.normal = asEdgeNorm[iVertex];
                    vtx.position = asEdgeVertex[iVertex];
                    buffer.push_back(vtx);
                }
            }
        }

        template <typename Isosurface>
        static void marching_tetrahedron(Isosurface fnc, buffer_t& buffer, float x, float y, float z, float scale) {
            using zap::maths::vec3f;
            static const auto& vertex_offset_tbl = surface_defs::vertex_offset_tbl;
            static const auto& tetra_cube_tbl = surface_defs::tetra_cube_tbl;

            int iVertex, iTetrahedron, iVertexInACube;
            vec3f asCubePosition[8];
            float  afCubeValue[8];
            vec3f asTetrahedronPosition[4];
            float  afTetrahedronValue[4];

            //Make a local copy of the cube's corner positions
            for(iVertex = 0; iVertex < 8; iVertex++) {
                asCubePosition[iVertex].x = x + vertex_offset_tbl[iVertex][0]*scale;
                asCubePosition[iVertex].y = y + vertex_offset_tbl[iVertex][1]*scale;
                asCubePosition[iVertex].z = z + vertex_offset_tbl[iVertex][2]*scale;
            }

            //Make a local copy of the cube's corner values
            for(iVertex = 0; iVertex < 8; iVertex++) {
                afCubeValue[iVertex] = fnc(asCubePosition[iVertex].x,
                                           asCubePosition[iVertex].y,
                                           asCubePosition[iVertex].z);
            }

            for(iTetrahedron = 0; iTetrahedron < 6; iTetrahedron++) {
                for(iVertex = 0; iVertex < 4; iVertex++) {
                    iVertexInACube = tetra_cube_tbl[iTetrahedron][iVertex];
                    asTetrahedronPosition[iVertex].x = asCubePosition[iVertexInACube].x;
                    asTetrahedronPosition[iVertex].y = asCubePosition[iVertexInACube].y;
                    asTetrahedronPosition[iVertex].z = asCubePosition[iVertexInACube].z;
                    afTetrahedronValue[iVertex] = afCubeValue[iVertexInACube];
                }
                marching_tetrahedron(fnc, buffer, asTetrahedronPosition, afTetrahedronValue);
            }
        }

        template <typename T>
        static void vGetColor(vec3<T>& colour, const vec3<T>& normal) {
            const auto& x = normal.x; const auto& y = normal.y; const auto& z = normal.z;
            colour.x = (x > 0 ? x : 0) + (y < 0 ? -.5*y : 0) + (z < 0 ? -.5*z : 0);
            colour.y = (y > 0 ? y : 0) + (z < 0 ? -.5*z : 0) + (x < 0 ? -.5*x : 0);
            colour.z = (z > 0 ? z : 0) + (x < 0 ? -.5*x : 0) + (y < 0 ? -.5*y : 0);
        }

        template <typename T>
        static T fGetOffset(T fValue1, T fValue2, T fValueDesired) {
            T fDelta = fValue2 - fValue1;
            if(fDelta == 0.0) return 0.5;
            return (fValueDesired - fValue1)/fDelta;
        }

        template <typename T, typename Isosurface>
        static void normal(Isosurface fnc, T x, T y, T z, vec3<T>& n) {
            n.x = fnc(x-0.01, y, z) - fnc(x+0.01, y, z);
            n.y = fnc(x, y-0.01, z) - fnc(x, y+0.01, z);
            n.z = fnc(x, y, z-0.01) - fnc(x, y, z+0.01);
            n.normalise();
        }
    };
}}

#endif //ZAP_SURFACE_HPP
