/* Created by Darren Otgaar on 2016/09/18. http://www.github.com/otgaard/zap */
#ifndef ZAP_MATERIAL_HPP
#define ZAP_MATERIAL_HPP

#include <maths/vec3.hpp>
#include <maths/vec4.hpp>

namespace zap { namespace renderer {

template <typename T>
struct material {
    using vec3 = maths::vec3<T>;
    using vec4 = maths::vec4<T>;

    material() = default;
    material(const material& rhs) = default;
    material(const vec3& Me, const vec3& Ma, const vec4& Md, const vec3& Ms, float Sexp) : emissive(Me), ambient(Ma),
                                                                                           diffuse(Md), specular(Ms),
                                                                                           specular_exp(Sexp) { }

    material& operator=(const material& rhs) = default;

    vec3 emissive;
    vec3 ambient;
    vec4 diffuse;
    vec3 specular;
    float specular_exp;
};

}}

#endif //ZAP_MATERIAL_HPP
