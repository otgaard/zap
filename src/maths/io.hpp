//
// Created by Darren Otgaar on 2016/05/26.
//

#ifndef ZAP_IO_HPP
#define ZAP_IO_HPP

#include <maths/maths.hpp>
#include <maths/geometry/rect.hpp>

namespace zap { namespace maths {
    template <typename T> ZAPMATHS_EXPORT std::ostream& operator<<(std::ostream& str, const vec2<T>& V);
    template <typename T> ZAPMATHS_EXPORT std::ostream& operator<<(std::ostream& str, const vec3<T>& V);
    template <typename T> ZAPMATHS_EXPORT std::ostream& operator<<(std::ostream& str, const vec4<T>& V);
    template <typename T> ZAPMATHS_EXPORT std::ostream& operator<<(std::ostream& str, const mat2<T>& M);
    template <typename T> ZAPMATHS_EXPORT std::ostream& operator<<(std::ostream& str, const mat3<T>& M);
    template <typename T> ZAPMATHS_EXPORT std::ostream& operator<<(std::ostream& str, const mat4<T>& M);

    namespace geometry {
        template <typename T> ZAPMATHS_EXPORT std::ostream& operator<<(std::ostream& str, const rect<T>& r);
    }
    // See io.cpp for explicit instantiations exported from libMaths
}}

#endif //ZAP_IO_HPP
