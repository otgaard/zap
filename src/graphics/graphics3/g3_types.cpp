//
// Created by Darren Otgaar on 2017/05/28.
//

#include <graphics/graphics2/g2_types.hpp>
#include <graphics/graphics3/g3_types.hpp>

namespace zap { namespace core {
using namespace zap::maths;
/*
template struct position<vec3f>;
template struct position<vec3i>;
template struct position<vec3s>;
template struct normal<vec3f>;
template struct tangent<vec3f>;
template struct bitangent<vec3f>;
template struct texcoord1<vec3f>;
template struct colour1<vec3f>;
template struct colour1<vec4f>;
template struct pointsize<float>;
*/

}}

namespace zap { namespace engine {
using namespace zap::core;
using namespace zap::maths;
using namespace zap::graphics;

template struct vertex<pos3f_t>;
template struct vertex<psize1_t>;
template struct vertex<pos3f_t, tex2f_t>;
template struct vertex<pos3f_t, col3b_t>;
template struct vertex<nor3f_t, psize1_t>;
template struct vertex<pos3f_t, nor3f_t, tex2f_t>;
template struct vertex<pos3f_t, col4f_t, psize1_t>;
template struct vertex<pos3f_t, nor3f_t, tex2f_t, col3f_t>;
template struct vertex<pos3f_t, nor3f_t, tan3f_t, tex2f_t>;

template class vertex_buffer<vtx_p3_t>;
template class vertex_buffer<vtx_ps1_t>;
template class vertex_buffer<vtx_p3t2_t>;
template class vertex_buffer<vtx_p3c3_t>;
template class vertex_buffer<vtx_n3ps1_t>;
template class vertex_buffer<vtx_p3n3t2_t>;
template class vertex_buffer<vtx_p3n3tn3t2_t>;

template class index_buffer<unsigned int, primitive_type::PT_TRIANGLES>;

template struct mesh<vertex_stream<vbuf_p3_t>, primitive_type::PT_TRIANGLES>;
template struct mesh<vertex_stream<vbuf_p3n3t2_t>, primitive_type::PT_TRIANGLES>;
template struct mesh<vertex_stream<vbuf_p3n3t2_t>, primitive_type::PT_TRIANGLES, ibuf_tri4_t>;
template struct mesh<vertex_stream<vbuf_p3n3tn3t2_t>, primitive_type::PT_TRIANGLES>;
template struct mesh<vertex_stream<vbuf_p3n3tn3t2_t>, primitive_type::PT_TRIANGLES, ibuf_tri4_t>;
template struct mesh<vertex_stream<vbuf_p3n3tn3t2_t>, primitive_type::PT_TRIANGLE_STRIP>;
template struct mesh<vertex_stream<vbuf_p3_t, vbuf_ps1_t>, primitive_type::PT_TRIANGLES, ibuf_tri4_t>;
template struct mesh<vertex_stream<vbuf_p3_t, vbuf_n3ps1_t>, primitive_type::PT_TRIANGLES, ibuf_tri4_t>;


}}