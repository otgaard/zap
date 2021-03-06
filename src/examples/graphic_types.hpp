//
// Created by Darren Otgaar on 2016/06/10.
//

#ifndef ZAP_GRAPHIC_TYPES_HPP
#define ZAP_GRAPHIC_TYPES_HPP

#include <maths/vec2.hpp>
#include <maths/vec3.hpp>
#include <maths/vec4.hpp>
#include <maths/mat2.hpp>
#include <maths/mat3.hpp>
#include <maths/mat4.hpp>
#include <maths/transform.hpp>
#include <engine/engine.hpp>
#include <engine/vertex_buffer.hpp>
#include <engine/index_buffer.hpp>
#include <engine/pixel_buffer.hpp>
#include <engine/framebuffer.hpp>
#include <engine/uniform_buffer.hpp>
#include <engine/program.hpp>
#include <engine/shader.hpp>
#include <engine/texture.hpp>
#include <engine/mesh.hpp>

#include <renderer/colour.hpp>
#include <renderer/camera.hpp>

/*
using namespace zap;
using namespace zap::maths;
using namespace zap::engine;
using namespace zap::renderer;

using pos2f_t = core::position<vec2f>;
using pos3f_t = core::position<vec3f>;
using nor3f_t = core::normal<vec3f>;
using tex2f_t = core::texcoord1<vec2f>;
using tex3f_t = core::texcoord1<vec3f>;
using col3f_t = core::colour1<vec3f>;
using col4f_t = core::colour1<vec4f>;
using psize1_t = core::pointsize<float>;

using vtx_p2_t = vertex<pos2f_t>;
using vtx_p2t2_t = vertex<pos2f_t, tex2f_t>;
using vtx_p3n3t2_t = vertex<pos3f_t, nor3f_t>;
using vtx_p3c4ps1_t = vertex<pos3f_t, col4f_t, psize1_t>;
using vtx_p3n3t2c3_t = vertex<pos3f_t, nor3f_t, tex2f_t, col3f_t>;

using vbuf_p2_t = vertex_buffer<vtx_p2_t, buffer_usage::BU_STATIC_DRAW>;
using vbuf_p2t2_t = vertex_buffer<vtx_p2t2_t, buffer_usage::BU_STATIC_DRAW>;

using mesh_p2t2_trifan_t = mesh<vertex_stream<vbuf_p2t2_t>, primitive_type::PT_TRIANGLE_FAN>;

#define GLSL(src) "#version 330 core\n" #src
*/
#endif //ZAP_GRAPHIC_TYPES_HPP
