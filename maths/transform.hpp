//
// Created by Darren Otgaar on 2016/06/26.
//

#ifndef ZAP_TRANSFORM_HPP
#define ZAP_TRANSFORM_HPP

#include "mat2.hpp"
#include "mat3.hpp"
#include "mat4.hpp"
#include <core/enumfield.hpp>

namespace zap { namespace maths {
    template <typename AffineT>
    class transform {
    protected:
        enum class transform_state {
            TS_IDENTITY = 1 << 0,   // Transform is identity
            TS_ROTSCALE = 1 << 1,   // Transform is scale + rotation matrix
            TS_UNISCALE = 1 << 2,   // Scale is uniform
            TS_SYNCED = 1 << 3,     // The transform (affine matrix_) has been synced, syncs on use
            TS_SYNCEDINV = 1 << 4   // The inverse transform (inv_matrix_) has been synced, syncs on use
        };

    public:
        using affine_t = AffineT;
        using type = typename affine_t::type;   // Int/Float/Double, etc
        using row_t = typename affine_t::row_t; // Matrix row type (M)
        using col_t = typename affine_t::col_t; // Matrix column type (N)
        using vec_t = typename affine_t::vec_t; // Vector type (N-1)
        using rot_t = typename affine_t::rot_t; // The rotation matrix

        transform(bool initialise=true) { if(initialise) make_identity(); }

        void make_identity();

        void translate(const vec_t& T);
        void rotate(const rot_t& R);        // An orthogonal rotation matrix
        void matrix(const rot_t& M);        // Arbitrary Rotation Matrix
        void uniform_scale(type S);
        void scale(const vec_t& S);

        const vec_t& translation();
        const vec_t& scale() const;
        type uniform_scale() const;
        const rot_t& rotation() const;
        const rot_t& matrix() const;
        const affine_t& affine() const;
        const affine_t& inv_affine() const;
        mat4<type> gl_matrix() const;

        transform operator*(const transform& rhs) const;

    protected:
        void update_transform() const;
        void invert_affine() const;

        rot_t rotation_;
        vec_t scale_;
        vec_t translation_;
        mutable affine_t matrix_;
        mutable affine_t inv_matrix_;
        mutable core::enumfield<int, transform_state> transform_state_;
    };

    using transform3f = transform<mat3f>;
    using transform4f = transform<mat4f>;

    template <typename AffineT>
    void transform<AffineT>::make_identity() {
        rotation_ = rot_t::identity();
        scale_ = vec_t(type(1));
        translation_ = vec_t(type(0));
        matrix_ = affine_t::identity();
        inv_matrix_ = affine_t::identity();
        transform_state_.set(transform_state::TS_IDENTITY, transform_state::TS_ROTSCALE, transform_state::TS_UNISCALE,
                             transform_state::TS_SYNCED, transform_state::TS_SYNCEDINV);
    }

    template <typename AffineT>
    void transform<AffineT>::translate(const vec_t& T) {
        transform_state_.clear(transform_state::TS_IDENTITY, transform_state::TS_SYNCED, transform_state::TS_SYNCEDINV);
        translation_ = T;
    }

    template <typename AffineT>
    void transform<AffineT>::rotate(const rot_t& R) {
        transform_state_.clear(transform_state::TS_IDENTITY, transform_state::TS_SYNCED, transform_state::TS_SYNCEDINV);
        transform_state_.set(transform_state::TS_ROTSCALE);
        rotation_ = R;
    }

    template <typename AffineT>
    void transform<AffineT>::matrix(const rot_t& M) {
        transform_state_.clear(transform_state::TS_IDENTITY, transform_state::TS_ROTSCALE, transform_state::TS_SYNCED, transform_state::TS_SYNCEDINV);
        rotation_ = M;
    }

    template <typename AffineT>
    void transform<AffineT>::uniform_scale(transform::type S) {
        assert(transform_state_.is_set(transform_state::TS_ROTSCALE) && "Transform must be Rotation/Scale Matrix");
        transform_state_.clear(transform_state::TS_IDENTITY, transform_state::TS_SYNCED, transform_state::TS_SYNCEDINV);
        transform_state_.set(transform_state::TS_UNISCALE);
        scale_ = vec_t(S);
    }

    template <typename AffineT>
    void transform<AffineT>::scale(const vec_t& S) {
        assert(transform_state_.is_set(transform_state::TS_ROTSCALE) && "Transform must be Rotation/Scale Matrix");
        transform_state_.clear(transform_state::TS_IDENTITY, transform_state::TS_UNISCALE, transform_state::TS_SYNCED, transform_state::TS_SYNCEDINV);
        scale_ = S;
    }

    template <typename AffineT>
    const typename transform<AffineT>::vec_t& transform<AffineT>::translation() {
        return translation_;
    }

    template <typename AffineT>
    const typename transform<AffineT>::vec_t& transform<AffineT>::scale() const {
        assert(transform_state_.is_set(transform_state::TS_ROTSCALE) && "Transform is not valid Rotation/Scale Matrix");
        return scale_;
    }

    template <typename AffineT>
    typename transform<AffineT>::type transform<AffineT>::uniform_scale() const {
        assert(transform_state_.is_set(transform_state::TS_ROTSCALE) &&
               transform_state_.is_set(transform_state::TS_UNISCALE) && "Transform is not valid uniform Rotation/Scale Matrix");
        return scale_[0];
    }

    template <typename AffineT>
    const typename transform<AffineT>::rot_t& transform<AffineT>::rotation() const {
        assert(transform_state_.is_set(transform_state::TS_ROTSCALE) && "Transform is not valid Rotation/Scale Matrix");
        return rotation_;
    }

    template <typename AffineT>
    const typename transform<AffineT>::rot_t& transform<AffineT>::matrix() const {
        return rotation_;
    }

    template <typename AffineT>
    const typename transform<AffineT>::affine_t& transform<AffineT>::affine() const {
        if(!transform_state_.is_set(transform_state::TS_SYNCED)) update_transform();
        return matrix_;
    }

    template <typename AffineT>
    const typename transform<AffineT>::affine_t& transform<AffineT>::inv_affine() const {
        if(!transform_state_.is_set(transform_state::TS_SYNCEDINV)) invert_affine();
        return inv_matrix_;
    }

    template <>
    mat4<typename transform<mat4f>::type> transform<mat4f>::gl_matrix() const {
        if(!transform_state_.is_set(transform_state::TS_SYNCED)) update_transform();
        return matrix_;
    }

    template <>
    mat4<typename transform<mat3f>::type> transform<mat3f>::gl_matrix() const {
        if(!transform_state_.is_set(transform_state::TS_SYNCED)) update_transform();
        mat4f m(1.f, 1.f, 1.f, 1.f);
        m.column(0, vec3f(matrix_.col2(0), 0.f));
        m.column(1, vec3f(matrix_.col2(1), 0.f));
        m.column(3, vec3f(matrix_.col2(2), 0.f));
        return m;
    }

    template <typename AffineT>
    void transform<AffineT>::update_transform() const {
        if(transform_state_.is_set(transform_state::TS_IDENTITY)) matrix_.identity();
        else {
            if(transform_state_.is_set(transform_state::TS_ROTSCALE)) {
                for(int c = 0, cend = affine_t::cols()-1; c != cend; ++c) {
                    for(int r = 0, rend = affine_t::rows()-1; r != rend; ++r) {
                        matrix_(r,c) = rotation_(r,c)*scale_[c];
                    }
                }
            } else {
                matrix_.rotation(rotation_);
            }

            matrix_.column(affine_t::cols()-1, translation_);
        }

        transform_state_.set(transform_state::TS_SYNCED);
    }

    template <typename AffineT>
    void transform<AffineT>::invert_affine() const {
        if(!transform_state_.is_set(transform_state::TS_SYNCED)) update_transform();

        if(transform_state_.is_set(transform_state::TS_IDENTITY)) {
            inv_matrix_.identity();
        } else {
            if(transform_state_.is_set(transform_state::TS_ROTSCALE)) {
                if(transform_state_.is_set(transform_state::TS_UNISCALE)) {
                    type inv_scale = type(1)/scale_[0];
                    for(int c = 0, cend = affine_t::cols()-1; c != cend; ++c) {
                        for(int r = 0, rend = affine_t::rows()-1; r != rend; ++r) {
                            inv_matrix_(r,c) = inv_scale*rotation_(c,r);
                        }
                    }
                } else {
                    auto inv_scale = reciprocal(scale_);

                    for(int c = 0, cend = affine_t::cols()-1; c != cend; ++c) {
                        for(int r = 0, rend = affine_t::rows()-1; r != rend; ++r) {
                            inv_matrix_(r,c) = inv_scale[r]*rotation_(c,r);
                        }
                    }
                }
            } else {
                inv_matrix_.rotation(rotation_.inverse());
            }

            matrix_.column(affine_t::cols()-1, translation_);
            constexpr auto col = affine_t::cols()-1;
            for(int r = 0, rend = affine_t::rows()-1; r != rend; ++r) {
                inv_matrix_(r,col) = -dot(inv_matrix_.row(r), translation_);
            }
        }
    }

    template <typename AFFINE_MAT_T>
    transform<AFFINE_MAT_T> transform<AFFINE_MAT_T>::operator*(const transform& rhs) const {
        if(!transform_state_.is_set(transform_state::TS_SYNCED))     update_transform();
        if(!rhs.transform_state_.is_set(transform_state::TS_SYNCED)) rhs.update_transform();

        if(transform_state_.is_set(transform_state::TS_IDENTITY))     return rhs;
        if(rhs.transform_state_.is_set(transform_state::TS_IDENTITY)) return *this;

        transform P;
        if(transform_state_.is_set(transform_state::TS_ROTSCALE) && transform_state_.is_set(transform_state::TS_ROTSCALE)) {
            if(transform_state_.is_set(transform_state::TS_UNISCALE)) {
                P.rotate(rotation_ * rhs.rotation_);
                P.translate(uniform_scale()*(rotation_*rhs.translation_) + translation_);
                if(rhs.transform_state_.is_set(transform_state::TS_UNISCALE)) P.uniform_scale(uniform_scale()*rhs.uniform_scale());
                else                                                          P.scale(uniform_scale()*rhs.scale());
                return P;
            }
        }

        rot_t M_scale(scale_);
        rot_t N_scale(rhs.scale_);

        rot_t A = transform_state_.is_set(transform_state::TS_ROTSCALE)     ? rotation_     * M_scale : rotation_;
        rot_t B = rhs.transform_state_.is_set(transform_state::TS_ROTSCALE) ? rhs.rotation_ * N_scale : rhs.rotation_;

        P.matrix(A*B);
        P.translate(A*rhs.translation_ + translation_);
        return P;
    }

}}

#endif //ZAP_TRANSFORM_HPP
