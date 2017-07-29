//
// Created by Darren Otgaar on 2016/10/10.
//

#ifndef ZAP_NODE_HPP
#define ZAP_NODE_HPP

#include <vector>
#include <memory>
#include "spatial.hpp"

namespace zap { namespace scene_graph {
    template <typename SpatialT, typename PtrT>
    class node : public SpatialT {
    public:
        using spatial_t = SpatialT;
        using ptr_t = PtrT;

        node() = default;
        explicit node(size_t reserve) : children_(reserve) { }
        node(const node& rhs) = delete;
        virtual ~node() = default;

        node& operator=(const node& rhs) = delete;

        void update(double t, float dt) override;

        size_t child_count() const { return children_.size(); }
        size_t attach_child(spatial_t* ptr);
        spatial_t* detach_child(spatial_t* ptr);
        spatial_t* detach_child(size_t idx);
        spatial_t* at(size_t idx);
        const spatial_t* at(size_t idx) const;

    protected:
        using child_array_t = std::vector<ptr_t>;
        void invalidate_transform() const override;
        void update_transform() const override;
        void update_bound() const override;

        child_array_t children_;
    };

    template <typename SpatialT, typename PtrT>
    size_t node<SpatialT, PtrT>::attach_child(typename node<SpatialT, PtrT>::spatial_t* ptr) {
        assert(ptr->parent() == nullptr && "Attempt to attach owned child");
        if(ptr->parent() != nullptr) return INVALID_IDX;
        auto idx = children_.size();
        children_.push_back(ptr);
        return idx;
    }

    template <typename SpatialT, typename PtrT>
    typename node<SpatialT, PtrT>::spatial_t* node<SpatialT, PtrT>::detach_child(typename node<SpatialT, PtrT>::spatial_t* ptr) {
        auto it = std::find_if(children_.begin(), children_.end(), ptr);
        if(it != children_.end()) {
            children_.erase(it);
            return ptr;
        }
        return nullptr;
    }

    template <typename SpatialT, typename PtrT>
    typename node<SpatialT, PtrT>::spatial_t* node<SpatialT, PtrT>::detach_child(size_t idx) {
        auto it = children_.begin()+idx;
        if(it != children_.end()) {
            auto ptr = *it;
            children_.erase(it);
            return ptr;
        }
        return nullptr;
    }

    template <typename SpatialT, typename PtrT>
    typename node<SpatialT, PtrT>::spatial_t* node<SpatialT, PtrT>::at(size_t idx) {
        return idx < children_.size() ? children_[idx] : nullptr;
    }

    template <typename SpatialT, typename PtrT>
    const typename node<SpatialT, PtrT>::spatial_t* node<SpatialT, PtrT>::at(size_t idx) const {
        return idx < children_.size() ? children_[idx] : nullptr;
    }

    template <typename SpatialT, typename PtrT>
    void node<SpatialT, PtrT>::update(double t, float dt) {
        if(SpatialT::cache_state_.is_set(spatial_state::SS_TRANS_INVALID)) {
            update_transform();

            for(auto spatial : children_) {
                spatial->invalidate_transform();
                spatial->update(t, dt);
            }
        } else {
            for(auto spatial : children_) {
                spatial->update(t, dt);
            }
        }
    }

    template <typename SpatialT, typename PtrT>
    void node<SpatialT, PtrT>::update_transform() const {
        if(SpatialT::parent_ && SpatialT::parent_->cache_state_.is_set(spatial_state::SS_TRANS_INVALID)) {
            SpatialT::parent_->update_transform();
            SpatialT::cache_state_.set(spatial_state::SS_TRANS_INVALID);
        }

        if(!SpatialT::cache_state_.is_set(spatial_state::SS_TRANS_INVALID)) return;

        if(SpatialT::parent_) {
            SpatialT::world_transform_ = SpatialT::parent_->world_transform() * SpatialT::model_transform();
            SpatialT::parent_->invalidate_bound();
        } else {
            SpatialT::world_transform_ = SpatialT::model_transform();
        }
        SpatialT::cache_state_.clear(spatial_state::SS_TRANS_INVALID);
        SpatialT::cache_state_.set(spatial_state::SS_BOUND_INVALID);
        for(auto spatial : children_) spatial->invalidate_transform();
    }

    template <typename SpatialT, typename PtrT>
    void node<SpatialT, PtrT>::update_bound() const {
        if(SpatialT::cache_state_.is_set(spatial_state::SS_BOUND_INVALID)) {
            bool found_first = false;
            for(auto spatial : children_) {
                if(found_first) SpatialT::world_bound_.combine(spatial->world_bound());
                else {
                    found_first = true;
                    SpatialT::world_bound_ = spatial->world_bound();
                }
            }
            SpatialT::model_bound_ = SpatialT::world_bound_.transform(SpatialT::world_transform().inv_affine());
        }
        SpatialT::cache_state_.clear(spatial_state::SS_BOUND_INVALID);
    }

    template <typename SpatialT, typename PtrT>
    void node<SpatialT, PtrT>::invalidate_transform() const {
        SpatialT::invalidate_transform();
        for(auto spatial : children_) spatial->invalidate_transform();
    }
}}

#endif //ZAP_NODE_HPP
