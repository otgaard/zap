/* Created by Darren Otgaar on 2016/10/29. http://www.github.com/otgaard/zap */
#ifndef ZAP_SAMPLER_HPP
#define ZAP_SAMPLER_HPP

#include "engine.hpp"

namespace zap { namespace engine {

class sampler {
public:
    sampler() : id_(INVALID_RESOURCE) { }
    sampler(sampler&& rhs) : id_(rhs.id_) { rhs.id_ = INVALID_RESOURCE; }
    sampler& operator=(sampler&& rhs) {
        if(this != &rhs) std::swap(id_, rhs.id_);
        return *this;
    }
    ~sampler() { if(is_allocated()) deallocate(); }

    sampler(const sampler& rhs) = delete;
    sampler& operator=(const sampler& rhs) = delete;

    bool allocate();
    void initialise();  // Set to defaults
    void deallocate();
    bool is_allocated() const { return id_ != INVALID_IDX; }

    void bind(uint32_t unit);
    void release(uint32_t unit);

    void set_wrap_s(tex_wrap w);
    void set_wrap_t(tex_wrap w);
    void set_wrap_r(tex_wrap w);

    void set_mag_filter(tex_filter f);
    void set_min_filter(tex_filter f);

    void set_anisotropy(float v);
    void set_border_colour(float r, float g, float b, float a);

private:
    resource_t id_;
};

}}

#endif //ZAP_SAMPLER_HPP
