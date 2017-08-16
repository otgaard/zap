//
// Created by Darren Otgaar on 2017/08/16.
//

#ifndef ZAP_STRING_HPP
#define ZAP_STRING_HPP

#include <sstream>
#include <tools/log.hpp>

namespace zap {
    template <typename T>
    std::string lexical_cast(const T& value) {
        std::ostringstream ostr;
        try {
            ostr << value;
            return ostr.str();
        } catch(const std::exception& e) {
            LOG_ERR("Failed to convert value to string:", e.what());
            return std::string{};
        }
    }
}

#endif //ZAP_STRING_HPP
