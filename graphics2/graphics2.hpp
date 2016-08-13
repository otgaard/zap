//
// Created by Darren Otgaar on 2016/08/07.
//

#ifndef ZAP_GRAPHICS2_HPP
#define ZAP_GRAPHICS2_HPP

#include <maths/vec2.hpp>
#include <maths/vec3.hpp>
#include <maths/vec4.hpp>
#include <maths/mat2.hpp>
#include <maths/mat3.hpp>
#include <maths/mat4.hpp>
#include <engine/vertex_buffer.hpp>
#include <engine/mesh.hpp>

namespace zap { namespace graphics2 {

using namespace zap::maths;
using namespace zap::engine;

using pos2f_t = core::position<vec2f>;
using tex2f_t = core::texcoord1<vec2f>;

using vtx_p2_t = vertex<pos2f_t>;
using vtx_p2t2_t = vertex<pos2f_t, tex2f_t>;

using vbuf_p2_t = vertex_buffer<vtx_p2_t, buffer_usage::BU_STATIC_DRAW>;
using vbuf_p2t2_t = vertex_buffer<vtx_p2t2_t, buffer_usage::BU_STATIC_DRAW>;

using mesh_p2t2_trifan_t = mesh<vertex_stream<vbuf_p2t2_t>, primitive_type::PT_TRIANGLE_FAN>;

}}

#endif //ZAP_GRAPHICS2_HPP
