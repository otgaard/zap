//
// Created by Darren Otgaar on 2017/02/08.
//

#ifndef ZAP_GRAPHICS3_TYPES_HPP
#define ZAP_GRAPHICS3_TYPES_HPP

#include <graphics/graphics2/g2_types.hpp>

namespace zap { namespace graphics {

using namespace zap::maths;
using namespace zap::engine;

using pos3f_t = core::position<vec3f>;
using pos4f_t = core::position<vec4f>;
using pos3i_t = core::position<vec3i>;
using pos3s_t = core::position<vec3s>;
using nor3f_t = core::normal<vec3f>;
using tan3f_t = core::tangent<vec3f>;
using bin3f_t = core::bitangent<vec3f>;
using tex3f_t = core::texcoord1<vec3f>;
using col3f_t = core::colour1<vec3f>;
using col4f_t = core::colour1<vec4f>;
using psize1_t = core::pointsize<float>;
using psize4_t = core::pointsize<vec4f>;    // pointsize, age, rotation, user-defined (for particles)

using vtx_p3_t = vertex<pos3f_t>;
using vtx_p4_t = vertex<pos4f_t>;
using vtx_c4_t = vertex<col4f_t>;
using vtx_ps1_t = vertex<psize1_t>;
using vtx_ps4_t = vertex<psize4_t>;
using vtx_p3t2_t = vertex<pos3f_t, tex2f_t>;
using vtx_p3n3_t = vertex<pos3f_t, nor3f_t>;
using vtx_p3c3_t = vertex<pos3f_t, col3b_t>;
using vtx_p3c4_t = vertex<pos3f_t, col4b_t>;
using vtx_p4c4_t = vertex<pos4f_t, col4f_t>;
using vtx_n3ps1_t = vertex<nor3f_t, psize1_t>;
using vtx_p3n3t2_t = vertex<pos3f_t, nor3f_t, tex2f_t>;
using vtx_p3c4t2_t = vertex<pos3f_t, col4b_t, tex2f_t>;
using vtx_p3c4ps1_t = vertex<pos3f_t, col4f_t, psize1_t>;
using vtx_p3n3t2c3_t = vertex<pos3f_t, nor3f_t, tex2f_t, col3f_t>;
using vtx_p3n3tn3t2_t = vertex<pos3f_t, nor3f_t, tan3f_t, tex2f_t>;
using vtx_p3c4t2ps4_t = vertex<pos3f_t, col4b_t, tex2f_t, psize4_t>;

using vbuf_p3_t = vertex_buffer<vtx_p3_t>;
using vbuf_p4_t = vertex_buffer<vtx_p4_t>;
using vbuf_c4_t = vertex_buffer<vtx_c4_t>;
using vbuf_ps1_t = vertex_buffer<vtx_ps1_t>;
using vbuf_ps4_t = vertex_buffer<vtx_ps4_t>;
using vbuf_p3t2_t = vertex_buffer<vtx_p3t2_t>;
using vbuf_p3n3_t = vertex_buffer<vtx_p3n3_t>;
using vbuf_p3c3_t = vertex_buffer<vtx_p3c3_t>;
using vbuf_p3c4_t = vertex_buffer<vtx_p3c4_t>;
using vbuf_p4c4_t = vertex_buffer<vtx_p4c4_t>;
using vbuf_n3ps1_t = vertex_buffer<vtx_n3ps1_t>;
using vbuf_p3c4t2_t = vertex_buffer<vtx_p3c4t2_t>;
using vbuf_p3n3t2_t = vertex_buffer<vtx_p3n3t2_t>;
using vbuf_p3n3tn3t2_t = vertex_buffer<vtx_p3n3tn3t2_t>;
using vbuf_p3c4t2ps4_t = vertex_buffer<vtx_p3c4t2ps4_t>;

using mesh_p3_t = mesh<vertex_stream<vbuf_p3_t>>;
using mesh_p3c4_t = mesh<vertex_stream<vbuf_p3c4_t>>;
using mesh_p3_u16_t = mesh<vertex_stream<vbuf_p3_t>, ibuf_u16_t>;
using mesh_p3_u32_t = mesh<vertex_stream<vbuf_p3_t>, ibuf_u32_t>;
using mesh_p3t2_t = mesh<vertex_stream<vbuf_p3t2_t>>;
using mesh_p3n3_t = mesh<vertex_stream<vbuf_p3n3_t>>;
using mesh_p3c4_t = mesh<vertex_stream<vbuf_p3c4_t>>;

using mesh_p3n3t2_t = mesh<vertex_stream<vbuf_p3n3t2_t>>;
using mesh_p3c4t2_t = mesh<vertex_stream<vbuf_p3c4t2_t>>;
using mesh_p3c4t2_u32_t = mesh<vertex_stream<vbuf_p3c4t2_t>, ibuf_u32_t>;
using mesh_p3n3t2_u32_t = mesh<vertex_stream<vbuf_p3n3t2_t>, ibuf_u32_t>;
using mesh_p3n3tn3t2_t = mesh<vertex_stream<vbuf_p3n3tn3t2_t>>;
using mesh_p3n3tn3t2_u32_t = mesh<vertex_stream<vbuf_p3n3tn3t2_t>, ibuf_u32_t>;

using mesh_p3_ps1_u32_t = mesh<vertex_stream<vbuf_p3_t, vbuf_ps1_t>, ibuf_u32_t>;
using mesh_p3_n3ps1_u32_t = mesh<vertex_stream<vbuf_p3_t, vbuf_n3ps1_t>, ibuf_u32_t>;
using mesh_p3c4_t2_u32_t = mesh<vertex_stream<vbuf_p3c4_t, vbuf_t2_t>, ibuf_u32_t>;

// Particle types (in separate vertex_buffers to make updating easier with vbuf_t2_t instanced)
using mesh_particles_t = mesh<vertex_stream<vbuf_p4_t, vbuf_t2_t>>;
using mesh_particles_col_t = mesh<vertex_stream<vbuf_p4_t, vbuf_c4_t, vbuf_t2_t>>;
using mesh_particles_col_age_t = mesh<vertex_stream<vbuf_p4_t, vbuf_c4_t, vbuf_ps1_t, vbuf_t2_t>>;
using mesh_particles_col_data_t = mesh<vertex_stream<vbuf_p4_t, vbuf_c4_t, vbuf_ps4_t, vbuf_t2_t>>;

}}

#endif //ZAP_GRAPHICS3_TYPES_HPP
