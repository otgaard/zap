//
// Created by Darren Otgaar on 2016/07/24.
//

#include <maths/transform.hpp>

namespace zap { namespace maths {
/*
template <>
typename transform<mat4f>::gltype_t transform<mat4f>::gl_matrix() const {
    if(!transform_state_.is_set(transform_state::TS_SYNCED)) update_transform();
    return matrix_;
}

template <>
typename transform<mat3f>::gltype_t transform<mat3f>::gl_matrix() const {
    if(!transform_state_.is_set(transform_state::TS_SYNCED)) update_transform();
    mat4f m(1.f, 1.f, 1.f, 1.f);
    m.column(0, vec3f(matrix_.col2(0), 0.f));
    m.column(1, vec3f(matrix_.col2(1), 0.f));
    m.column(3, vec3f(matrix_.col2(2), 0.f));
    return m;
}

template <>
typename transform<mat4f>::gltype_t transform<mat4f>::gl_inverse() const {
    if(!transform_state_.is_set(transform_state::TS_SYNCEDINV)) invert_affine();
    return inv_matrix_;
}

template <>
typename transform<mat3f>::gltype_t transform<mat3f>::gl_inverse() const {
    if(!transform_state_.is_set(transform_state::TS_SYNCEDINV)) invert_affine();
    mat4f m(1.f, 1.f, 1.f, 1.f);
    m.column(0, vec3f(inv_matrix_.col2(0), 0.f));
    m.column(1, vec3f(inv_matrix_.col2(1), 0.f));
    m.column(3, vec3f(inv_matrix_.col2(2), 0.f));
    return m;
}

template class ZAPMATHS_EXPORT transform<mat3f>;
template class ZAPMATHS_EXPORT transform<mat3d>;
template class ZAPMATHS_EXPORT transform<mat4f>;
template class ZAPMATHS_EXPORT transform<mat4d>;
*/

}}
