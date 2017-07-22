//
// Created by Darren Otgaar on 2017/05/28.
//

#include "g2_types.hpp"

namespace zap { namespace core {
using namespace zap::maths;
/*
template struct position<vec2f>;
template struct position<vec2i>;
template struct position<vec2s>;
template struct texcoord1<vec2f>;
template struct texcoord1<vec2i>;
template struct texcoord1<vec2s>;
template struct colour1<vec3b>;
template struct colour1<vec4b>;
*/

}}

namespace zap { namespace engine {
using namespace zap::core;
using namespace zap::maths;
using namespace zap::graphics;

template struct vertex<pos2f_t>;
template struct vertex<pos2f_t, tex2f_t>;
template struct vertex<pos2f_t, col3b_t>;
template struct vertex<pos2f_t, col4b_t>;

template class vertex_buffer<vtx_p2_t>;
template class vertex_buffer<vtx_p2t2_t>;
template class vertex_buffer<vtx_p2c3_t>;

template struct mesh<vertex_stream<vbuf_p2_t>, primitive_type::PT_TRIANGLE_FAN>;
template struct mesh<vertex_stream<vbuf_p2t2_t>, primitive_type::PT_TRIANGLE_FAN>;
template struct mesh<vertex_stream<vbuf_p2c3_t>, primitive_type::PT_LINE_STRIP>;
template struct mesh<vertex_stream<vbuf_plot_t>, primitive_type::PT_LINES>;

}}