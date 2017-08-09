/* Created by Darren Otgaar on 2016/08/14. http://www.github.com/otgaard/zap */
#include "renderer.hpp"

zap::renderer::renderer* zap::renderer::renderer::inst_ = nullptr;

zap::renderer::renderer* zap::renderer::renderer::instance() {
    if(!inst_) inst_ = new renderer{};
    return inst_;
}


bool zap::renderer::renderer::initialise() {
    initialised_ = state_stack_.initialise();
    return initialised_;
}