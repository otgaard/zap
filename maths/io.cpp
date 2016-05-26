//
// Created by Darren Otgaar on 2016/05/26.
//

#include "io.hpp"
#include <iomanip>
#include <iostream>

namespace zap { namespace maths {

}}

template std::ostream& zap::maths::operator<< <int>(std::ostream& str, const zap::maths::mat4<int>& M);
template std::ostream& zap::maths::operator<< <float>(std::ostream& str, const zap::maths::mat4<float>& M);
template std::ostream& zap::maths::operator<< <double>(std::ostream& str, const zap::maths::mat4<double>& M);
