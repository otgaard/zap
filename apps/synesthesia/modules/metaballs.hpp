/* Created by Darren Otgaar on 2016/06/11. http://www.github.com/otgaard/zap */
#ifndef ZAP_METABALLS_HPP
#define ZAP_METABALLS_HPP

#include "module.hpp"

class metaballs : public module {
public:
    metaballs(application* app_ptr);
    virtual ~metaballs();

    virtual void update(double t, float dt);
    virtual void draw();

    float eval_surface(float x, float y, float z);

protected:
    struct state_t;
    std::unique_ptr<state_t> state;
};


#endif //ZAP_METABALLS_HPP
