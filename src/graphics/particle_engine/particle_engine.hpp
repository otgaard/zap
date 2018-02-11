/* Created by Darren Otgaar on 2016/10/28. http://www.github.com/otgaard/zap */
#ifndef ZAP_PARTICLE_ENGINE_HPP
#define ZAP_PARTICLE_ENGINE_HPP

#include <memory>
#include <graphics/graphics.hpp>
#include "renderer/renderer_fwd.hpp"

/*
 * A shader-backed particle engine used for simulating millions of particles
 */

namespace zap { namespace effects {

class ZAPGRAPHICS_EXPORT particle_engine {
public:
    particle_engine();
    ~particle_engine();

    particle_engine(const particle_engine& rhs) = delete;
    particle_engine& operator=(const particle_engine& rhs) = delete;

    bool initialise();

    void update(double t, float dt);
    void draw(const renderer::camera& cam);

protected:
    bool setup_buffers();
    bool setup_shaders();

private:
    struct state_t;
    std::unique_ptr<state_t> state_;
    state_t& s;
};

}}

#endif //ZAP_PARTICLE_ENGINE_HPP
