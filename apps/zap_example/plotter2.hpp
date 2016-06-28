/* Created by Darren Otgaar on 2016/06/26. http://www.github.com/otgaard/zap */
#ifndef ZAP_PLOTTER2_HPP
#define ZAP_PLOTTER2_HPP

#include <memory>

// A simple 2D plotting class for graphing.



class plotter2 {
public:
    plotter2();
    ~plotter2();

    bool initialise();
    void draw();

private:
    struct state_t;
    std::unique_ptr<state_t> state_;
};


#endif //ZAP_PLOTTER2_HPP
