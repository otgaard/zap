//
// Created by Darren Otgaar on 2016/10/10.
//

#ifndef ZAP_NODE_HPP
#define ZAP_NODE_HPP

#include <vector>
#include "spatial.hpp"

namespace zap { namespace engine { namespace scene_graph {
    template <typename SpatialT>
    class node : public SpatialT {
    public:
        using spatial_t = SpatialT;

        node() = default;
        node(size_t reserve) : children_(reserve) { }
        node(const node& rhs) = delete;
        virtual ~node() = default;

        node& operator=(const node& rhs) = delete;

        virtual void update(double t, float dt) override;

        size_t child_count() const { return childen_.size(); }
        size_t attach_child(spatial_t* ptr);
        spatial_t detach_child(spatial_t* ptr);
        spatial_t detach_child(size_t idx);
        spatial_t* at(size_t idx);
        const spatial* at(size_t idx) const;

    protected:
        using child_arr_t = std::vector<spatial_t*>;
        virtual void invalidate_transform() const override;
        virtual void update_transform() const override;
        virtual void update_bound() const override;

        child_arr_t children_;
    };

    template <typename SpatialT>
    size_t node<SpatialT>::attach_child(spatial_t* ptr) {
        assert(ptr->parent() == nullptr && "Attempt to attach owned child");
        if(ptr->parent() != nullptr) return INVALID_IDX;
        auto idx = children_.size();
        children_.push_back(ptr);
        return idx;
    }

    template <typename SpatialT>
    typename node<SpatialT>::spatial_t* node<SpatialT>::detach_child(spatial_t* ptr) {
        auto it = std::find_if(children_.begin(), children_.end(), ptr);
        if(it != children_.end()) {
            children_.erase(it);
            return ptr;
        }
        return nullptr;
    }

    template <typename SpatialT>
    typename node<SpatialT>::spatial_t* node<SpatialT>::detach_child(size_t idx) {
        auto it = children_.begin()+idx;
        if(it != children_.end()) {
            auto ptr = *it;
            children_.erase(it);
            return ptr;
        }
        return nullptr;
    }

    template <typename SpatialT>
    typename node<SpatialT>::spatial_t* node<SpatialT>::at(size_t idx) {
        if(idx < children_.size()) return children_[idx];
        else                       return nullptr;
    }

    template <typename SpatialT>
    const typename node<SpatialT>::spatial_t* node<SpatialT>::at(size_t idx) const {
        if(idx < children_.size()) return children_[idx];
        else                       return nullptr;
    }

    template <typename SpatialT>
    void node<SpatialT>::update(double t, float dt) {
        if(cache_state_.is_set(spatial_state::SS_TRANS_INVALID)) {
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

    template <typename SpatialT>
    void node<SpatialT>::update_transform() const {
        if(parent_ && parent_->cache_state_.is_set(spatial_state::SS_TRANS_INVALID)) {
            parent_->update_transform();
            cache_state_.set(spatial_state::SS_TRANS_INVALID);
        }

        if(!cache_state_.is_set(spatial_state::SS_TRANS_INVALID)) return;

        if(parent_) {
            world_transform_ = parent_->world_transform() * model_transform();
            parent_->invalidate_bound();
        } else {
            world_transform_ = model_transform();
        }
        cache_state_.clear(spatial_state::SS_TRANS_INVALID);
        cache_state_.set(spatial_state::SS_BOUND_INVALID);
        for(auto spatial : children_) spatial->invalidate_transform();
    }

    template <typename SpatialT>
    void node<SpatialT>::update_bound() const {
        if(cache_state_.is_set(spatial_state::SS_BOUND_INVALID)) {
            bool found_first = false;
            for(auto spatial : children_) {
                if(found_first) world_bound_.combine(spatial->world_bound());
                else {
                    found_first = true;
                    world_bound_ = spatial->world_bound();
                }
            }
            model_bound_ = world_bound_.transform(world_transform().inv_affine());
        }
        cache_state_.clear(spatial_state::SS_BOUND_INVALID);
    }

    template <typename SpatialT>
    void node<SpatialT>::invalidate_transform() const {
        SpatialT::invalidate_transform();
        for(auto spatial : children_) spatial->invalidate_transform();
    }
}}}

#endif //ZAP_NODE_HPP
