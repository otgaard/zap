//
// Created by Darren Otgaar on 2017/02/08.
//

#ifndef ZAP_GRAPHICS3_TYPES_HPP
#define ZAP_GRAPHICS3_TYPES_HPP

#include "graphics2/g2_types.hpp"

namespace zap { namespace graphics {

using namespace zap::maths;
using namespace zap::engine;

using pos3f_t = core::position<vec3f>;
using pos3i_t = core::position<vec3i>;
using pos3s_t = core::position<vec3s>;
using nor3f_t = core::normal<vec3f>;
using tex3f_t = core::texcoord1<vec3f>;
using col3f_t = core::colour1<vec3f>;
using col4f_t = core::colour1<vec4f>;
using psize1_t = core::pointsize<float>;

using vtx_p3_t = vertex<pos3f_t>;
using vtx_p3t2_t = vertex<pos3f_t, tex2f_t>;
using vtx_p3c3_t = vertex<pos3f_t, col3b_t>;
using vtx_p3n3t2_t = vertex<pos3f_t, nor3f_t, tex2f_t>;
using vtx_p3c4ps1_t = vertex<pos3f_t, col4f_t, psize1_t>;
using vtx_p3n3t2c3_t = vertex<pos3f_t, nor3f_t, tex2f_t, col3f_t>;

using vbuf_p3_t = vertex_buffer<vtx_p3_t>;
using vbuf_p3t2_t = vertex_buffer<vtx_p3t2_t>;
using vbuf_p3c3_t = vertex_buffer<vtx_p3c3_t>;
using vbuf_p3n3t2_t = vertex_buffer<vtx_p3n3t2_t>;

using ibuf_tri4_t = index_buffer<unsigned int, primitive_type::PT_TRIANGLES>;

using mesh_p3_tri_t = mesh<vertex_stream<vbuf_p3_t>, primitive_type::PT_TRIANGLES>;
using mesh_p3n3t2_tri_t = mesh<vertex_stream<vbuf_p3n3t2_t>, primitive_type::PT_TRIANGLES>;
using mesh_p3n3t2_trii_t = mesh<vertex_stream<vbuf_p3n3t2_t>, primitive_type::PT_TRIANGLES, ibuf_tri4_t>;

}}

#endif //ZAP_GRAPHICS3_TYPES_HPP
