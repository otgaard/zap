//
// Created by Darren Otgaar on 2017/07/30.
//

#ifndef ZAP_RENDER_CONTEXT_HPP
#define ZAP_RENDER_CONTEXT_HPP

/*
 * The render_context is a container for all the state associated with the mesh (vbuf + ibuf) required to render the
 * visual.  Therefore, the render_context is a strongly-typed container for the shaders, program, uniforms, uniform
 * buffers, textures, render_state, and other values required to draw the object on screen.
 *
 * Long-term goals for the render_context:
 *
 * 1) Binding and usage as fast as possible
 * 2) Support multiple passes
 * 3) Queryable, enumerable, & programmable state (program uniforms, uniform buffers, textures, render_state)
 * 4) State sharing or ownership of resources
 * 5)

 */

#include <memory>
#include <engine/program.hpp>
#include <engine/texture.hpp>
#include <engine/sampler.hpp>
#include <engine/framebuffer.hpp>
#include <engine/render_state.hpp>
#include <engine/uniform_buffer.hpp>

namespace zap { namespace renderer {

using program_ptr = std::unique_ptr<engine::program>;
using texture_ptr = std::unique_ptr<engine::texture>;
using sampler_ptr = std::unique_ptr<engine::sampler>;
using framebuffer_ptr = std::unique_ptr<engine::framebuffer>;
using render_state_ptr = std::unique_ptr<engine::render_state>;

class render_context {
public:
    using program = engine::program;
    using texture = engine::texture;
    using sampler = engine::sampler;
    using framebuffer = engine::framebuffer;
    using render_state = engine::render_state;

    render_context() = default;
    explicit render_context(program* prog) : program_{prog} { }
    render_context(program* prog, texture* tex) : program_{prog}, textures_{tex} { }

    void set_program(program* ptr) { program_ = ptr; }
    program* get_program() const { return program_; }


    void bind() const;
    void release() const;

protected:

private:
    // no ownership yet
    program* program_;
    std::vector<texture*> textures_;
    std::vector<sampler*> samplers_;

    std::vector<char> uniforms_;            // Store all uniforms in a contiguous block
};



}}

#endif //ZAP_RENDER_CONTEXT_HPP
