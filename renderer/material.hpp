/* Created by Darren Otgaar on 2016/09/18. http://www.github.com/otgaard/zap */
#ifndef ZAP_MATERIAL_HPP
#define ZAP_MATERIAL_HPP

#include <maths/algebra.hpp>
#include <engine/uniform_buffer.hpp>

namespace zap { namespace renderer {
    using material_block = engine::uniform_block<
        core::material_emissive<maths::vec4f>,
        core::material_ambient<maths::vec4f>,
        core::material_diffuse<maths::vec4f>,
        core::material_specular<maths::vec4f>
    >;

    struct material {

    };
}}

#endif //ZAP_MATERIAL_HPP
