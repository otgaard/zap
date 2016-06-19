//
// Created by Darren Otgaar on 2016/03/22.
//

#ifndef ZAP_PROGRAM_HPP
#define ZAP_PROGRAM_HPP

#include <vector>
#include "engine.hpp"
#include "shader.hpp"
#include <maths/algebra.hpp>

namespace zap { namespace engine {
    class program {
    public:
        program();
        program(const std::vector<shader_ptr>& shaders);
        program(std::vector<shader_ptr>&& shaders);
        program(program&& rhs);
        program& operator=(program&& rhs);
        ~program();

        std::int32_t uniform_location(const char* name);
        std::int32_t uniform_block_index(const char* name);

        resource_t resource() const { return id_; }
        bool is_allocated() const { return id_ != INVALID_RESOURCE; }
        bool is_linked() const { return is_allocated() && linked_; }

        void bind();
        void release();

        void add_shader(shader* shdr) { add_shader(std::shared_ptr<shader>(shdr)); }
        void add_shader(shader_ptr shdr) { shaders_.push_back(shdr); }
        size_t shader_count() const { return shaders_.size(); }
        shader_ptr get_shader(size_t idx) const {
            assert(idx < shaders_.size() && ZERR_IDX_OUT_OF_RANGE);
            return shaders_[idx];
        }
        bool link(bool clear=true);

        template <typename T> void bind_uniform(int location, const T& type);
        template <typename T> void bind_uniform(const char* name, const T& type);
        void bind_texture_unit(int location, int unit);

    protected:
        resource_t id_;
        bool linked_;
        std::vector<shader_ptr> shaders_;

    private:
        program(const program&);
        program& operator=(const program&);
    };

    using program_ptr = std::shared_ptr<program>;

    inline program::program() : id_(INVALID_RESOURCE), linked_(false) { }
    inline program::program(const std::vector<shader_ptr>& shaders) : id_(INVALID_RESOURCE), linked_(false),
                                                                      shaders_(std::move(shaders)) { }
    inline program::program(std::vector<shader_ptr>&& shaders) : id_(INVALID_RESOURCE), linked_(false),
                                                                 shaders_(std::move(shaders)) { }
    inline program::program(program&& rhs) : id_(rhs.id_), linked_(rhs.linked_), shaders_(std::move(rhs.shaders_)) {
        rhs.id_ = INVALID_RESOURCE; rhs.linked_ = false;
    }
    inline program& program::operator=(program&& rhs) {
        if(this != &rhs) {
            id_ = rhs.id_; linked_ = rhs.linked_; shaders_ = std::move(rhs.shaders_);
            rhs.id_ = INVALID_RESOURCE; rhs.linked_ = false;
        }
        return *this;
    }

    template <> void program::bind_uniform<int>(int location, const int& value);
    template <> void program::bind_uniform<zap::maths::vec3f>(int location, const zap::maths::vec3f& type);
    template <> void program::bind_uniform<zap::maths::mat4f>(int location, const zap::maths::mat4f& type);

    template <> void program::bind_uniform<int>(const char* name, const int& value);
    template <> void program::bind_uniform<zap::maths::vec3f>(const char* name, const zap::maths::vec3f& type);
    template <> void program::bind_uniform<zap::maths::mat4f>(const char* name, const zap::maths::mat4f& type);



}}

#endif //ZAP_PROGRAM_HPP
