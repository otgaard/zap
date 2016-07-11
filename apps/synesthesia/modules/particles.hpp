/* Created by Darren Otgaar on 2016/07/06. http://www.github.com/otgaard/zap */
#ifndef ZAP_PARTICLES_HPP
#define ZAP_PARTICLES_HPP

#include "module.hpp"

class particles : public module {
public:
    particles(application* app_ptr);
    virtual ~particles();

    virtual void update(double t, float dt);
    virtual void draw();

protected:
    struct state_t;
    std::unique_ptr<state_t> state;
};

#endif //ZAP_PARTICLES_HPP
