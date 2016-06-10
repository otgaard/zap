/* Created by Darren Otgaar on 2016/06/10. http://www.github.com/otgaard/zap */
#ifndef ZAP_BARS_HPP
#define ZAP_BARS_HPP

#include "../module.hpp"

class bars : public module {
public:
    bars();
    virtual ~bars();

    virtual void update(double t, float dt);
    virtual void draw();

protected:
    struct state_t;
    std::unique_ptr<state_t> state;
};

#endif //ZAP_BARS_HPP
