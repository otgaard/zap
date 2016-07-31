/* Created by Darren Otgaar on 2016/07/24. http://www.github.com/otgaard/zap */
#ifndef ZAP_QUAD_HPP
#define ZAP_QUAD_HPP

#include "apps/graphic_types.hpp"

class quad {
public:
    quad();
    ~quad();

    void set_texture(texture&& tex);

    bool initialise();
    void update(double t, float dt);
    void draw(const camera& cam);

    transform3f world_transform;

private:
    program program_;
    texture texture_;
    vbuf_p2t2_t vbuf_;
    mesh_p2t2_trifan_t mesh_;
};

#endif //ZAP_QUAD_HPP
