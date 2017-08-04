/* Created by Darren Otgaar on 2016/08/07. http://www.github.com/otgaard/zap */
#ifndef ZAP_QUAD_HPP
#define ZAP_QUAD_HPP

#include "g2_types.hpp"
#include "engine/program.hpp"
#include "engine/texture.hpp"
#include "maths/transform.hpp"
#include "renderer/camera.hpp"

namespace zap { namespace graphics {

class quad {
public:
    quad();
    ~quad();

    void set_texture(engine::texture&& tex);
    engine::texture& get_texture() { return texture_; }

    bool initialise(engine::shader* frag_shdr=nullptr);
    bool initialise(const std::string& frag_shdr_src);
    void update(double t, float dt);
    void draw();

    void set_override(const engine::texture* override) { tex_override_ = override; }
    const engine::texture* get_override() const { return tex_override_; }
    void clear_override() { tex_override_ = nullptr; }

    int get_width() const { return screen_.x; }
    int get_height() const { return screen_.y; }

    engine::program* get_program() { return &program_; }
    void bind() { program_.bind(); }
    void release() { program_.release(); }

    void resize(int w, int h);

private:
    engine::program program_;
    engine::shader* frag_shdr_ = nullptr;
    const engine::texture* tex_override_ = nullptr;
    engine::texture texture_;
    vbuf_p2t2_t vbuf_;
    mesh_p2t2_tfan_t mesh_;
    vec2i screen_ = {0, 0};
    int32_t curr_viewport_[4] = {0, 0, 0, 0};
};

}}

#endif //ZAP_QUAD_HPP
