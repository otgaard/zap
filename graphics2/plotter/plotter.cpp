/* Created by Darren Otgaar on 2016/11/27. http://www.github.com/otgaard/zap */
#include "plotter.hpp"

using namespace zap;
using namespace graphics;

struct plotter::state_t {

};

plotter::plotter() : state_(new state_t()), s(*state_.get()) {

}

plotter::plotter(const vec2f& domain, const vec2f& range, const float majors) : state_(new state_t()), s(*state_.get()) {

}

plotter::~plotter() {

}

bool plotter::initialise() {
    return true;
}

void plotter::update(double t, float dt) {

}

void plotter::draw(const renderer::camera& cam) {

}
