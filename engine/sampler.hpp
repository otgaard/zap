/* Created by Darren Otgaar on 2016/10/29. http://www.github.com/otgaard/zap */
#ifndef ZAP_SAMPLER_HPP
#define ZAP_SAMPLER_HPP

#include "engine.hpp"

namespace zap { namespace engine {

class sampler {
public:
    sampler();
    ~sampler();

    sampler(const sampler& rhs) = delete;
    sampler& operator=(const sampler& rhs) = delete;

    bool initialise();

    void bind();
    void release();


protected:

private:
    resource_t id_;

};

}}

#endif //ZAP_SAMPLER_HPP
