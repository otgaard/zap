/* Created by Darren Otgaar on 2016/08/14. http://www.github.com/otgaard/zap */
#include "renderer.hpp"

bool zap::renderer::renderer::initialise() {
    initialised_ = state_stack_.initialise();
    return initialised_;
}