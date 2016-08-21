/* Created by Darren Otgaar on 2016/08/07. http://www.github.com/otgaard/zap */
#ifndef ZAP_QUAD_HPP
#define ZAP_QUAD_HPP

#include "graphics2.hpp"
#include "engine/program.hpp"
#include "engine/texture.hpp"
#include "maths/transform.hpp"
#include "renderer/camera.hpp"

namespace zap { namespace graphics2 {

class quad {
public:
    quad();
    ~quad();

    void set_texture(engine::texture&& tex);
    engine::texture& get_texture() { return texture_; }

    bool initialise();
    void update(double t, float dt);
    void draw();

    void resize(int w, int h) { screen_.set(w,h); }

private:
    engine::program program_;
    engine::texture texture_;
    vbuf_p2t2_t vbuf_;
    mesh_p2t2_trifan_t mesh_;
    vec2i screen_;
};

}}

#endif //ZAP_QUAD_HPP
