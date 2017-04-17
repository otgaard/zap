//
// Created by Darren Otgaar on 2016/08/07.
//

#ifndef ZAP_GRAPHICS2_HPP
#define ZAP_GRAPHICS2_HPP

#include <engine/mesh.hpp>
#include <maths/algebra.hpp>
#include <engine/index_buffer.hpp>
#include <engine/vertex_buffer.hpp>

namespace zap { namespace graphics {

using namespace zap::maths;
using namespace zap::engine;

using pos2f_t = core::position<vec2f>;
using pos2i_t = core::position<vec2i>;
using pos2s_t = core::position<vec2s>;
using tex2f_t = core::texcoord1<vec2f>;
using tex2i_t = core::texcoord1<vec2i>;
using tex2s_t = core::texcoord1<vec2s>;
using col3b_t = core::colour1<vec3b>;
using col4b_t = core::colour1<vec4b>;

using vtx_p2_t = vertex<pos2f_t>;
using vtx_p2t2_t = vertex<pos2f_t, tex2f_t>;
using vtx_p2c3_t = vertex<pos2f_t, col3b_t>;
using vtx_p2c4_t = vertex<pos2f_t, col4b_t>;

using vbuf_p2_t = vertex_buffer<vtx_p2_t>;
using vbuf_p2t2_t = vertex_buffer<vtx_p2t2_t>;
using vbuf_p2c3_t = vertex_buffer<vtx_p2c3_t>;

using vbuf_plot_t = vertex_buffer<vtx_p2c3_t, buffer_usage::BU_DYNAMIC_COPY>;

using mesh_p2t2_tfan_t = mesh<vertex_stream<vbuf_p2t2_t>, primitive_type::PT_TRIANGLE_FAN>;
using mesh_p2c3_ls_t = mesh<vertex_stream<vbuf_p2c3_t>, primitive_type::PT_LINE_STRIP>;
using mesh_plot_ls_t = mesh<vertex_stream<vbuf_plot_t>, primitive_type::PT_LINES>;

}}

#define GLSL(src) "#version 330 core\n" #src

#endif //ZAP_GRAPHICS2_HPP