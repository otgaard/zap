//
// Created by otgaard on 2016/03/08.
//

#ifndef ZAP_CORE_HPP
#define ZAP_CORE_HPP

#include <cstdint>

constexpr std::int32_t INVALID_IDX = -1;

#define ZERR_IDX_OUT_OF_RANGE "Index out of range"
#define ZERR_ARG_COUNT_INVALID "Incorrect number of values provided"
#define ZERR_TYPE_INVALID "Invalid type used for template instantiation"
#define ZERR_TYPE_FLOATING "Requires floating-point type"

#define UNUSED(expr) do { (void)(expr); } while (0)

#endif //ZAP_CORE_HPP
