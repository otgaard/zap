//
// Created by Darren Otgaar on 2016/04/09.
//

#ifndef ZAP_VERTEX_ATTRIBUTE_HPP
#define ZAP_VERTEX_ATTRIBUTE_HPP

#include "engine.hpp"
#include <core/pod.hpp>
#include <maths/maths.hpp>

namespace zap {
    namespace core {
        MAKE_PODFIELD(position, engine::attribute_type, engine::attribute_type::AT_POSITION);
        MAKE_PODFIELD(normal, engine::attribute_type, engine::attribute_type::AT_NORMAL);
        MAKE_PODFIELD(tangent, engine::attribute_type, engine::attribute_type::AT_TANGENT);
        MAKE_PODFIELD(binormal, engine::attribute_type, engine::attribute_type::AT_BINORMAL);
        MAKE_PODFIELD(texcoord1, engine::attribute_type, engine::attribute_type::AT_TEXCOORD1);
        MAKE_PODFIELD(texcoord2, engine::attribute_type, engine::attribute_type::AT_TEXCOORD2);
        MAKE_PODFIELD(texcoord3, engine::attribute_type, engine::attribute_type::AT_TEXCOORD3);
        MAKE_PODFIELD(texcoord4, engine::attribute_type, engine::attribute_type::AT_TEXCOORD4);
        MAKE_PODFIELD(texcoord5, engine::attribute_type, engine::attribute_type::AT_TEXCOORD5);
        MAKE_PODFIELD(texcoord6, engine::attribute_type, engine::attribute_type::AT_TEXCOORD6);
        MAKE_PODFIELD(colour1, engine::attribute_type, engine::attribute_type::AT_COLOUR1);
        MAKE_PODFIELD(colour2, engine::attribute_type, engine::attribute_type::AT_COLOUR2);
        MAKE_PODFIELD(blendindex, engine::attribute_type, engine::attribute_type::AT_BLENDINDEX);
        MAKE_PODFIELD(blendweight, engine::attribute_type, engine::attribute_type::AT_BLENDWEIGHT);
        MAKE_PODFIELD(fogcoord, engine::attribute_type, engine::attribute_type::AT_FOGCOORD);
        MAKE_PODFIELD(pointsize, engine::attribute_type, engine::attribute_type::AT_POINTSIZE);
    }
}

namespace zap { namespace engine {

}}

#endif //ZAP_VERTEX_ATTRIBUTE_HPP
