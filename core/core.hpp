//
// Created by otgaard on 2016/03/08.
//

#ifndef ZAP_CORE_HPP
#define ZAP_CORE_HPP

#include <cstdint>
#include <type_traits>

constexpr std::uint32_t INVALID_IDX = std::uint32_t(-1);

#define ZERR_IDX_OUT_OF_RANGE "Index out of range"
#define ZERR_ARG_COUNT_INVALID "Incorrect number of values provided"
#define ZERR_TYPE_INVALID "Invalid type used for template instantiation"
#define ZERR_TYPE_FLOATING "Requires floating-point type"

#define UNUSED(expr) do { (void)(expr); } while (0)

template <template <typename...> class TYPE, typename T>
struct is_specialisation_of : std::false_type { };

template <template <typename...> class TYPE, typename... P>
struct is_specialisation_of<TYPE, TYPE<P...>> : std::true_type { };

#endif //ZAP_CORE_HPP
