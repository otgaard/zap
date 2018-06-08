//
// Created by Darren Otgaar on 2016/05/26.
//

#include <maths/io.hpp>
#include <iomanip>
#include <iostream>
#include <maths/vec2.hpp>
#include <maths/vec3.hpp>
#include <maths/vec4.hpp>
#include <maths/mat2.hpp>
#include <maths/mat3.hpp>
#include <maths/mat4.hpp>
#include <maths/geometry/rect.hpp>

static constexpr size_t default_precision = 6;

namespace zap { namespace maths {

template <typename T>
std::ostream& operator<<(std::ostream& str, const vec2<T>& V) {
    str << std::fixed << std::setprecision(default_precision);
    str << "[" << V[0] << ", " << V[1] << "]";
    return str;
}

template <typename T>
std::ostream& operator<<(std::ostream& str, const vec3<T>& V) {
    str << std::fixed << std::setprecision(default_precision);
    str << "[" << V[0] << ", " << V[1] << ", " << V[2] << "]";
    return str;
}

template <typename T>
std::ostream& operator<<(std::ostream& str, const vec4<T>& V) {
    str << std::fixed << std::setprecision(default_precision);
    str << "[" << V[0] << ", " << V[1] << ", " << V[2] << ", " << V[3] << "]";
    return str;
}

template <typename T>
std::ostream& operator<<(std::ostream& str, const mat2<T>& M) {
    str << std::fixed << std::setprecision(default_precision) << std::endl;
    str << M(0,0) << "   " << M(0,1) << std::endl;
    str << M(1,0) << "   " << M(1,1) << std::endl;
    return str;
}

template <typename T>
std::ostream& operator<<(std::ostream& str, const mat3<T>& M) {
    str << std::fixed << std::setprecision(default_precision) << std::endl;
    str << M(0,0) << "   " << M(0,1) << "   " << M(0,2) << std::endl;
    str << M(1,0) << "   " << M(1,1) << "   " << M(1,2) << std::endl;
    str << M(2,0) << "   " << M(2,1) << "   " << M(2,2) << std::endl;
    return str;
}

template <typename T>
std::ostream& operator<<(std::ostream& str, const mat4<T>& M) {
    str << std::fixed << std::setprecision(default_precision) << std::endl;
    str << M(0,0) << "   " << M(0,1) << "   " << M(0,2) << "   " << M(0,3) << std::endl;
    str << M(1,0) << "   " << M(1,1) << "   " << M(1,2) << "   " << M(1,3) << std::endl;
    str << M(2,0) << "   " << M(2,1) << "   " << M(2,2) << "   " << M(2,3) << std::endl;
    str << M(3,0) << "   " << M(3,1) << "   " << M(3,2) << "   " << M(3,3) << std::endl;
    return str;
}

template ZAPMATHS_EXPORT std::ostream& operator<< <uint8_t>(std::ostream& str, const vec2<uint8_t>& V);
template ZAPMATHS_EXPORT std::ostream& operator<< <int16_t>(std::ostream& str, const vec2<int16_t>& V);
template ZAPMATHS_EXPORT std::ostream& operator<< <int32_t>(std::ostream& str, const vec2<int32_t>& V);
template ZAPMATHS_EXPORT std::ostream& operator<< <float>(std::ostream& str, const vec2<float>& V);
template ZAPMATHS_EXPORT std::ostream& operator<< <double>(std::ostream& str, const vec2<double>& V);
template ZAPMATHS_EXPORT std::ostream& operator<< <uint8_t>(std::ostream& str, const vec3<uint8_t>& V);
template ZAPMATHS_EXPORT std::ostream& operator<< <int16_t>(std::ostream& str, const vec3<int16_t>& V);
template ZAPMATHS_EXPORT std::ostream& operator<< <int32_t>(std::ostream& str, const vec3<int32_t>& V);
template ZAPMATHS_EXPORT std::ostream& operator<< <float>(std::ostream& str, const vec3<float>& V);
template ZAPMATHS_EXPORT std::ostream& operator<< <double>(std::ostream& str, const vec3<double>& V);
template ZAPMATHS_EXPORT std::ostream& operator<< <uint8_t>(std::ostream& str, const vec4<uint8_t>& V);
template ZAPMATHS_EXPORT std::ostream& operator<< <int16_t>(std::ostream& str, const vec4<int16_t>& V);
template ZAPMATHS_EXPORT std::ostream& operator<< <int32_t>(std::ostream& str, const vec4<int32_t>& V);
template ZAPMATHS_EXPORT std::ostream& operator<< <float>(std::ostream& str, const vec4<float>& V);
template ZAPMATHS_EXPORT std::ostream& operator<< <double>(std::ostream& str, const vec4<double>& V);
template ZAPMATHS_EXPORT std::ostream& operator<< <uint8_t>(std::ostream& str, const mat2<uint8_t>& V);
template ZAPMATHS_EXPORT std::ostream& operator<< <int16_t>(std::ostream& str, const mat2<int16_t>& V);
template ZAPMATHS_EXPORT std::ostream& operator<< <int32_t>(std::ostream& str, const mat2<int32_t>& V);
template ZAPMATHS_EXPORT std::ostream& operator<< <float>(std::ostream& str, const mat2<float>& V);
template ZAPMATHS_EXPORT std::ostream& operator<< <double>(std::ostream& str, const mat2<double>& V);
template ZAPMATHS_EXPORT std::ostream& operator<< <uint8_t>(std::ostream& str, const mat3<uint8_t>& V);
template ZAPMATHS_EXPORT std::ostream& operator<< <int16_t>(std::ostream& str, const mat3<int16_t>& V);
template ZAPMATHS_EXPORT std::ostream& operator<< <int32_t>(std::ostream& str, const mat3<int32_t>& V);
template ZAPMATHS_EXPORT std::ostream& operator<< <float>(std::ostream& str, const mat3<float>& V);
template ZAPMATHS_EXPORT std::ostream& operator<< <double>(std::ostream& str, const mat3<double>& V);
template ZAPMATHS_EXPORT std::ostream& operator<< <uint8_t>(std::ostream& str, const mat4<uint8_t>& V);
template ZAPMATHS_EXPORT std::ostream& operator<< <int16_t>(std::ostream& str, const mat4<int16_t>& V);
template ZAPMATHS_EXPORT std::ostream& operator<< <int32_t>(std::ostream& str, const mat4<int32_t>& V);
template ZAPMATHS_EXPORT std::ostream& operator<< <float>(std::ostream& str, const mat4<float>& V);
template ZAPMATHS_EXPORT std::ostream& operator<< <double>(std::ostream& str, const mat4<double>& V);

namespace geometry {

template <typename T>
std::ostream& operator<<(std::ostream& str, const geometry::rect<T>& r) {
    str << std::fixed << std::setprecision(default_precision);
    str << "rect: [" << r.left << ", " << r.bottom << "] -> [" << r.right << ", " << r.top << "]" << std::endl;
    return str;
}

template ZAPMATHS_EXPORT std::ostream& operator<< <int16_t>(std::ostream& str, const rect<int16_t>& r);
template ZAPMATHS_EXPORT std::ostream& operator<< <int32_t>(std::ostream& str, const recti& r);
template ZAPMATHS_EXPORT std::ostream& operator<< <float>(std::ostream& str, const rectf& r);

}}}

