//
// Created by Darren Otgaar on 2016/05/26.
//

#ifndef ZAP_IO_HPP
#define ZAP_IO_HPP

#include <iosfwd>
#include <iomanip>
#include "mat4.hpp"

namespace zap { namespace maths {
    template <typename T>
    std::ostream& operator<<(std::ostream& str, const mat4<T>& M) {
        str << std::fixed << std::setprecision(9) << std::endl;
        str << M(0,0) << "   " << M(0,1) << "   " << M(0,2) << "   " << M(0,3) << std::endl;
        str << M(1,0) << "   " << M(1,1) << "   " << M(1,2) << "   " << M(1,3) << std::endl;
        str << M(2,0) << "   " << M(2,1) << "   " << M(2,2) << "   " << M(2,3) << std::endl;
        str << M(3,0) << "   " << M(3,1) << "   " << M(3,2) << "   " << M(3,3) << std::endl;
        return str;
    }

    template<> std::ostream& operator<< <int>(std::ostream& str, const mat4<int>& M);
    //template<> std::ostream& operator<< <float>(std::ostream& str, const mat4<float>& M);
    template<> std::ostream& operator<< <double>(std::ostream& str, const mat4<double>& M);
}}

#endif //ZAP_IO_HPP
