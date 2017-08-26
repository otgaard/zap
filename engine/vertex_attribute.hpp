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
        MAKE_PODFIELD(bitangent, engine::attribute_type, engine::attribute_type::AT_BITANGENT);
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

        MAKE_PODFIELD(loc0, engine::generic_attribute_type, engine::generic_attribute_type::AT_LOCATION_0);
        MAKE_PODFIELD(loc1, engine::generic_attribute_type, engine::generic_attribute_type::AT_LOCATION_1);
        MAKE_PODFIELD(loc2, engine::generic_attribute_type, engine::generic_attribute_type::AT_LOCATION_2);
        MAKE_PODFIELD(loc3, engine::generic_attribute_type, engine::generic_attribute_type::AT_LOCATION_3);
        MAKE_PODFIELD(loc4, engine::generic_attribute_type, engine::generic_attribute_type::AT_LOCATION_4);
        MAKE_PODFIELD(loc5, engine::generic_attribute_type, engine::generic_attribute_type::AT_LOCATION_5);
        MAKE_PODFIELD(loc6, engine::generic_attribute_type, engine::generic_attribute_type::AT_LOCATION_6);
        MAKE_PODFIELD(loc7, engine::generic_attribute_type, engine::generic_attribute_type::AT_LOCATION_7);
        MAKE_PODFIELD(loc8, engine::generic_attribute_type, engine::generic_attribute_type::AT_LOCATION_8);
        MAKE_PODFIELD(loc9, engine::generic_attribute_type, engine::generic_attribute_type::AT_LOCATION_9);
        MAKE_PODFIELD(loc10, engine::generic_attribute_type, engine::generic_attribute_type::AT_LOCATION_10);
        MAKE_PODFIELD(loc11, engine::generic_attribute_type, engine::generic_attribute_type::AT_LOCATION_11);
        MAKE_PODFIELD(loc12, engine::generic_attribute_type, engine::generic_attribute_type::AT_LOCATION_12);
        MAKE_PODFIELD(loc13, engine::generic_attribute_type, engine::generic_attribute_type::AT_LOCATION_13);
        MAKE_PODFIELD(loc14, engine::generic_attribute_type, engine::generic_attribute_type::AT_LOCATION_14);
        MAKE_PODFIELD(loc15, engine::generic_attribute_type, engine::generic_attribute_type::AT_LOCATION_15);
    }
}

namespace zap { namespace engine {

}}

#endif //ZAP_VERTEX_ATTRIBUTE_HPP
