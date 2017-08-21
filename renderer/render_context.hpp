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
 *
 * Notes:
 *
 * 1) It might make sense to store the resource_t directly as a parameter and then bind it without requiring reference
 *    to the texture or sampler.  That would allow textures to be set more like parameters but would move the ownership
 *    problem elsewhere.
 *
 * TODO:
 * 1) Provide better ownership model (make ptr_t a template parameter?).
 * 2) Allow specialisation based on what the context needs (i.e. don't provide textures_ if no textures are used)
 * 3) Provide verification and warnings when using context with missing parameters/blocks/textures/etc.
 */

#include <memory>
#include <cstring>
#include <algorithm>
#include <engine/program.hpp>
#include <engine/texture.hpp>
#include <engine/sampler.hpp>
#include <engine/render_state.hpp>
#include <engine/uniform_buffer.hpp>

namespace zap { namespace renderer {

class renderer;
class render_args;

using program_ptr = std::unique_ptr<engine::program>;
using texture_ptr = std::unique_ptr<engine::texture>;
using sampler_ptr = std::unique_ptr<engine::sampler>;
using render_state_ptr = std::unique_ptr<engine::render_state>;

class render_context {
public:
    using parameter = engine::parameter;
    using block = engine::block;
    using program = engine::program;
    using texture = engine::texture;
    using sampler = engine::sampler;
    using ubuffer_base = engine::ubuffer_base;
    using render_state = engine::render_state;

    render_context() = default;
    explicit render_context(program* prog) : program_{prog} { }
    render_context(const std::string& vshdr, const std::string& fshdr) : program_{new program{vshdr, fshdr}}, owns_program_(true) { }
    render_context(program* prog, texture* tex) : program_{prog}, textures_{tex} { }
    render_context(const render_context&) = delete; // for now
    render_context(render_context&& rhs) noexcept : program_(rhs.program_), rndr_state_(rhs.rndr_state_),
        parameters_(std::move(rhs.parameters_)), blocks_(std::move(rhs.blocks_)), textures_(std::move(rhs.textures_)),
        samplers_(std::move(rhs.samplers_)), ubname_(std::move(rhs.ubname_)), ubuffers_(std::move(rhs.ubuffers_)),
        offsets_(std::move(rhs.offsets_)), uniforms_(std::move(rhs.uniforms_)), dirty_flags_(std::move(rhs.dirty_flags_)),
        dirty_(rhs.dirty_), is_bound_(rhs.is_bound_), owns_program_(rhs.owns_program_) {
        rhs.owns_program_ = false;
    }

    ~render_context() {
        if(owns_program_) delete program_;
    }

    render_context& operator=(const render_context&) = delete;
    render_context& operator=(render_context&& rhs) noexcept {
        if(this != &rhs) {
            program_ = rhs.program_;
            rndr_state_ = rhs.rndr_state_;
            parameters_ = std::move(rhs.parameters_);
            blocks_ = std::move(rhs.blocks_);
            textures_ = std::move(rhs.textures_);
            samplers_ = std::move(rhs.samplers_);
            ubname_ = std::move(rhs.ubname_);
            ubuffers_ = std::move(rhs.ubuffers_);
            offsets_ = std::move(rhs.offsets_);
            uniforms_ = std::move(rhs.uniforms_);
            dirty_flags_ = std::move(rhs.dirty_flags_);
            dirty_ = rhs.dirty_;
            is_bound_ = rhs.is_bound_;
            owns_program_ = rhs.owns_program_;
            rhs.owns_program_ = false;
        }
        return *this;
    }

    bool is_bound() const { return is_bound_; }

    bool initialise() {
        if(!program_) return false;
        if(!program_->is_allocated()) {
            if(!program_->link()) return false;
        }

        blocks_ = program_->get_uniform_blocks();

        parameters_ = program_->get_parameters();
        size_t total = 0;
        offsets_.resize(parameters_.size(), 0);
        for(size_t i = 0; i != parameters_.size(); ++i) {
            offsets_[i] = int(total);
            total += parameters_[i].count * parameters_[i].bytesize();
        }
        uniforms_.resize(total);
        dirty_flags_.resize(parameters_.size(), true);
        dirty_ = true;
        return true;
    }

    int32_t get_index(const std::string& name) const {
        auto it = std::find_if(parameters_.begin(), parameters_.end(), [&name](const auto& parm) { return parm.name == name; });
        if(it != parameters_.end()) return int32_t(it - parameters_.begin());
        else                        return -1;
    }

    parameter get_parameter(int idx) const {
        assert(0 <= idx && idx < parameters_.size() && "Invalid parameter index specified");
        return idx != -1 ? parameters_[idx] : parameter{-1};
    }

    parameter get_parameter(const std::string& name) const {
        return get_parameter(get_index(name));
    }

    int get_offset(int idx) const {
        assert(0 <= idx && idx < parameters_.size() && "Invalid parameter index specified");
        return idx != -1 ? offsets_[idx] : -1;
    }

    int get_offset(const std::string& name) const {
        return get_offset(get_index(name));
    }

    bool has_parameter(const std::string& name) const { return get_index(name) != -1; }

    void set_texture_unit(int idx, int unit) {
        assert(idx < int(parameters_.size()) && "Invalid parameter specified");
        assert(unit < int(textures_.size()) && "No texture specified for requested unit");

        auto& parm = parameters_[idx];
        int offset = offsets_[idx];
        if(parm.is_sampler()) {
            memcpy(uniforms_.data()+offset, &unit, parm.bytesize() * parm.count);
            if(is_bound_) {
                program_->bind_uniform(parm.location, parm.type, parm.count, &uniforms_[offset]);
            } else {
                dirty_flags_[idx] = true;
                dirty_ = true;
            }
        }
    }

    void set_texture_unit(const std::string& name, int unit) {
        auto it = std::find_if(parameters_.begin(), parameters_.end(), [&name](const auto& parm) { return parm.name == name; });
        if(it != parameters_.end()) set_texture_unit(int(it - parameters_.begin()), unit);
        else                        LOG_ERR("Parameter does not exist in program:", LOG_GREEN, name);
    }

    template <typename T>
    void set_parameter(int idx, const T* value) {
        assert(idx < int(parameters_.size()) && "Invalid parameter specified");
        auto& parm = parameters_[idx];
        int offset = offsets_[idx];
        if(((engine::parameter_type)engine::pt_descriptor<T>::value) == parm.type) {
            memcpy(uniforms_.data()+offset, value, parm.bytesize() * parm.count);
            if(is_bound_) {
                program_->bind_uniform(parm.location, parm.type, parm.count, &uniforms_[offset]);
            } else {
                dirty_flags_[idx] = true;
                dirty_ = true;
            }
        } else {
            LOG_ERR("Attempted to bind invalid type to parameter:", parm.name);
        }
    }

    template <typename T>
    void set_parameter(int idx, const T& value) {
        set_parameter(idx, &value);
    }

    template <typename T>
    void set_parameter(int idx, const std::initializer_list<T>& value) {
        set_parameter(idx, value.begin());
    }

    template <typename T>
    void set_parameter(const std::string& name, const T* value) {
        auto it = std::find_if(parameters_.begin(), parameters_.end(), [&name](const auto& parm) { return parm.name == name; });
        if(it != parameters_.end()) set_parameter(int(it - parameters_.begin()), value);
        else                        LOG_ERR("Parameter does not exist in program:", LOG_GREEN, name);
    }

    template <typename T>
    void set_parameter(const std::string& name, const T& value) {
        set_parameter(name, &value);
    }

    template <typename T>
    void set_parameter(const std::string& name, const std::initializer_list<T>& value) {
        set_parameter(name, value.begin());
    }

    void add_sampler(const texture* tex_ptr, const sampler* smp_ptr) {
        textures_.push_back(tex_ptr);
        samplers_.push_back(smp_ptr);
    }
    template <typename... Args>
    void add_sampler(const texture* tex_ptr, const sampler* smp_ptr, Args... ptrs) {
        textures_.push_back(tex_ptr);
        samplers_.push_back(smp_ptr);
        add_sampler(ptrs...);
    }

    void add_texture(const texture* tex_ptr) {
        textures_.push_back(tex_ptr);
    }

    template <typename... Args>
    void add_texture(const texture* tex_ptr, Args... ptrs) {
        textures_.push_back(tex_ptr);
        add_texture(ptrs...);
    }

    void add_uniform_buffer(const std::string& name, const ubuffer_base* ubuf_ptr) {
        ubname_.push_back(name);
        ubuffers_.emplace_back(ubuf_ptr);
    }

    template <typename... Args>
    void add_uniform_buffer(const std::string& name, const ubuffer_base* ubuf_ptr, Args... ptrs) {
        ubname_.push_back(name);
        ubuffers_.push_back(ubuf_ptr);
        add_uniform_buffer(ptrs...);
    }

    void set_program(program* ptr) { program_ = ptr; }
    void set_program(const std::string& vshdr, const std::string& fshdr) {
        if(program_ && owns_program_) delete program_;
        program_ = new program{vshdr, fshdr};
        owns_program_ = true;
    }
    program* get_program() const { return program_; }
    const std::vector<char>& get_uniforms() const { return uniforms_; }

    void set_state(const render_state* rndr_state) { rndr_state_ = rndr_state; }
    const render_state* get_state() const { return rndr_state_; }

    void bind() const;
    void bind(const render_args& args) const;
    void rebind() const;
    void release() const;

protected:

private:
    program* program_ = nullptr;
    const render_state* rndr_state_ = nullptr;  // The render_state for this context (may be nullptr)
    std::vector<parameter> parameters_;         // move this to a lookup table in the engine later (per program)
    std::vector<block> blocks_;                 // uniform blocks used by this program
    std::vector<const texture*> textures_;
    std::vector<const sampler*> samplers_;
    std::vector<std::string> ubname_;           // The names of the buffers
    std::vector<const ubuffer_base*> ubuffers_;
    std::vector<int> offsets_;                  // Store the offset of each parameter (-1) for none
    std::vector<char> uniforms_;                // Store all uniforms in a contiguous block
    mutable std::vector<bool> dirty_flags_;     // A set of dirty flags for each parameter
    mutable bool dirty_ = true;                 // If any uniform has been set on the client but not yet on the server
    mutable bool is_bound_ = false;
    bool owns_program_ = false;                 // If set, this context owns the program and must delete it
};

}}

#endif //ZAP_RENDER_CONTEXT_HPP
