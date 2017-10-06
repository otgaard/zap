//
// Created by Darren Otgaar on 2016/10/07.
//

#ifndef ZAP_HIT_RECORD_HPP
#define ZAP_HIT_RECORD_HPP

#include <maths/vec3.hpp>
#include <renderer/material.hpp>

namespace zap { namespace renderer {

template <typename T>
struct hit_record {
    using vec3 = maths::vec3<T>;

    hit_record() : hit(false), material_ptr(nullptr) { }
    hit_record(const hit_record& rhs) = default;

    hit_record& operator=(const hit_record& rhs) = delete;

    bool hit;
    vec3 world_coord;
    vec3 normal;
    material* material_ptr;
};

}}

#endif //ZAP_HIT_RECORD_HPP
