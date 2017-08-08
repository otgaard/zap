//
// Created by Darren Otgaar on 2016/10/10.
//

#ifndef ZAP_SPATIAL_HPP
#define ZAP_SPATIAL_HPP

#include <core/enumfield.hpp>
#include <maths/transform.hpp>
#include <scene_graph/bound.hpp>

namespace zap { namespace scene_graph {
    template <typename SpatialT, typename PtrT=std::unique_ptr<SpatialT>> class node;

    enum class cull_mode : char {
        CM_NEVER,
        CM_DYNAMIC,
        CM_ALWAYS
    };

    enum class spatial_state {
        SS_VALID = 0,
        SS_TRANS_INVALID = 1 << 0,
        SS_BOUND_INVALID = 1 << 1
    };

    template <typename TransformT, typename GeoT>
    class spatial {
    public:
        using transform_t = TransformT;
        using bound_t = bound<GeoT, TransformT>;
        using type = typename transform_t::type;
        using affine_t = typename transform_t::affine_t;
        using vec_t = typename transform_t::vec_t;
        using rot_t = typename transform_t::rot_t;

        spatial(const spatial& rhs) = delete;
        virtual ~spatial() = default;
        spatial& operator=(const spatial& rhs) = delete;

        void translate(const vec_t& T);
        void rotate(const rot_t& R);
        void rotate(const affine_t& R);
        void matrix(const rot_t& M);
        void uniform_scale(type S);
        void scale(const vec_t& S);

        const transform_t& model_transform() const { return model_transform_; }
        const bound_t& model_bound() const { return model_bound_; }
        const transform_t& world_transform() const;
        const bound_t& world_bound() const;

        void set_model_transform(const transform_t& T) { model_transform_ = T; invalidate_transform(); }

        const cull_mode culling() const { return culling_; }
        void culling(cull_mode c) { culling_ = c; }
        bool always_cull() const { return culling_ == cull_mode::CM_ALWAYS; }

        const spatial* parent() const { return parent_; }

        virtual void update(double t, float dt) { }

        // TODO: Remove temporary code.  The scene graph should not draw itself, it should be drawn by the renderer (after culling, ordering, etc)
        virtual void draw() const = 0;

    protected:
        friend class node<spatial>;

        spatial() = default;
        void set_parent(spatial* parent) { parent_ = parent; }
        void invalidate_bound() const { cache_state_.set(spatial_state::SS_BOUND_INVALID); }
        virtual void invalidate_transform() const {
            cache_state_.set(spatial_state::SS_TRANS_INVALID, spatial_state::SS_BOUND_INVALID);
            if(parent_) parent_->invalidate_bound();
        }

        virtual void update_transform() const;
        virtual void update_bound() const;

        spatial* parent_ = nullptr;
        transform_t model_transform_;
        mutable bound_t model_bound_;
        mutable core::enumfield<int, spatial_state> cache_state_;
        mutable transform_t world_transform_;
        mutable bound_t world_bound_;
        cull_mode culling_ = cull_mode::CM_DYNAMIC;
    };

    template <typename TransformT, typename GeoT>
    void spatial<TransformT,GeoT>::translate(const vec_t& T) {
        model_transform_.translate(T);
        invalidate_transform();
    };

    template <typename TransformT, typename GeoT>
    void spatial<TransformT,GeoT>::rotate(const rot_t& R) {
        model_transform_.rotate(R);
        invalidate_transform();
    }

    template <typename TransformT, typename GeoT>
    void spatial<TransformT,GeoT>::rotate(const affine_t& R) {
        model_transform_.rotate(R);
        invalidate_transform();
    }

    template <typename TransformT, typename GeoT>
    void spatial<TransformT,GeoT>::matrix(const rot_t& M) {
        model_transform_.matrix(M);
        invalidate_transform();
    }

    template <typename TransformT, typename GeoT>
    void spatial<TransformT,GeoT>::uniform_scale(type S) {
        model_transform_.uniform_scale(S);
        invalidate_transform();
    }

    template <typename TransformT, typename GeoT>
    void spatial<TransformT,GeoT>::scale(const vec_t& S) {
        model_transform_.scale(S);
        invalidate_transform();
    }

    template <typename TransformT, typename GeoT>
    const TransformT& spatial<TransformT,GeoT>::world_transform() const {
        update_transform();
        return world_transform_;
    }

    template <typename TransformT, typename GeoT>
    const typename spatial<TransformT,GeoT>::bound_t& spatial<TransformT,GeoT>::world_bound() const {
        update_bound();
        return world_bound_;
    }

    template <typename TransformT, typename GeoT>
    void spatial<TransformT,GeoT>::update_transform() const {
        if(parent_ && parent_->cache_state_.is_set(spatial_state::SS_TRANS_INVALID)) {
            parent_->update_transform();
            cache_state_.set(spatial_state::SS_TRANS_INVALID);
        }

        if(!cache_state_.is_set(spatial_state::SS_TRANS_INVALID)) return;

        if(parent_) world_transform_ = parent_->world_transform() * model_transform_;
        else        world_transform_ = model_transform_;
        cache_state_.clear(spatial_state::SS_TRANS_INVALID);
        cache_state_.set(spatial_state::SS_BOUND_INVALID);
        if(parent_) parent_->invalidate_bound();
    }

    template <typename TransformT, typename GeoT>
    void spatial<TransformT,GeoT>::update_bound() const {
        if(!cache_state_.is_set(spatial_state::SS_BOUND_INVALID)) return;

        if(cache_state_.is_set(spatial_state::SS_TRANS_INVALID)) update_transform();
        world_bound_ = model_bound_.transform(world_transform_);
        cache_state_.clear(spatial_state::SS_BOUND_INVALID);
    }

}}

#endif //ZAP_SPATIAL_HPP
