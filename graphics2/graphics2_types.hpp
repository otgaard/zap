//
// Created by Darren Otgaar on 2016/08/07.
//

#ifndef ZAP_GRAPHICS2_HPP
#define ZAP_GRAPHICS2_HPP

#include <maths/algebra.hpp>

#include <engine/vertex_buffer.hpp>
#include <engine/index_buffer.hpp>
#include <engine/mesh.hpp>

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

using vbuf_p2_t = vertex_buffer<vtx_p2_t, buffer_usage::BU_STATIC_DRAW>;
using vbuf_p2t2_t = vertex_buffer<vtx_p2t2_t, buffer_usage::BU_STATIC_DRAW>;

using mesh_p2t2_tfan_t = mesh<vertex_stream<vbuf_p2t2_t>, primitive_type::PT_TRIANGLE_FAN>;

}}

#endif //ZAP_GRAPHICS2_HPP
