/* Created by Darren Otgaar on 2016/07/02. http://www.github.com/otgaard/zap */
#ifndef ZAP_GRAPHS_HPP
#define ZAP_GRAPHS_HPP

#include "module.hpp"

class graphs : public module {
public:
    graphs(application* app_ptr);
    virtual ~graphs();

    virtual void update(double t, float dt);
    virtual void draw();

protected:
    struct state_t;
    std::unique_ptr<state_t> state;
};


#endif //ZAP_GRAPHS_HPP
