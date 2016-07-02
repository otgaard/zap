/* Created by Darren Otgaar on 2016/07/02. http://www.github.com/otgaard/zap */
#include "graphs.hpp"
#include "apps/graphic_types.hpp"

#include <graphics2/plotter2.hpp>

using namespace zap;
using namespace zap::maths;
using namespace zap::engine;

struct graphs::state_t {
    graphics2::plotter2 plotter;
};

graphs::graphs(application* app_ptr) : module(app_ptr, "graphs"), state(new state_t) {
    state->plotter.initialise();
}

graphs::~graphs() {
}

void graphs::update(double t, float dt) {
    state->plotter.push_value(analysis_.estimated_beat);
}

void graphs::draw() {
    state->plotter.draw();
}