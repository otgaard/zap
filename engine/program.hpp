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
    struct block {
        int32_t index = -1;
        std::string name;
        int32_t size;

        bool is_valid() const { return index != -1; }

        block() = default;
        explicit block(int32_t i) : index(i) { }
        block(int32_t i, const std::string& n, int32_t s) : index(i), name(n), size(s) { }
    };

    struct parameter {
        int32_t location = -1;
        std::string name;
        parameter_type type;
        int32_t count;

        bool is_valid() const { return location != -1; }
        bool is_array() const { return count > 1; }
        size_t bytesize() const { return pt_bytesize[int(type)]; }

        parameter() = default;
        explicit parameter(int32_t l) : location(l) { }
        parameter(int32_t l, const std::string& n, parameter_type t, int32_t c) : location(l), name(n), type(t), count(c) { }
    };

    class program {
    public:
        program();
        explicit program(const std::vector<shader_ptr>& shaders);
        explicit program(std::vector<shader_ptr>&& shaders);
        program(const std::string& vshdr, const std::string& fshdr);
        program(program&& rhs) noexcept;
        program& operator=(program&& rhs) noexcept;
        ~program();

        program(const program&) = delete;
        program& operator=(const program&) = delete;

        std::int32_t uniform_location(const char* name);
        std::int32_t uniform_block_index(const char* name);

        resource_t resource() const { return id_; }
        bool is_allocated() const { return id_ != 0; } // glCreateProgram returns 0 on error
        bool is_linked() const { return is_allocated() && linked_; }

        void bind() const;
        void release() const;

        std::vector<parameter> get_parameters() const;
        std::vector<block> get_uniform_blocks() const;

        void add_shader(shader_type type, const std::string& src) { add_shader(std::make_shared<shader>(type,src)); }
        void add_shader(shader* shdr) { add_shader(std::shared_ptr<shader>(shdr)); }
        void add_shader(shader_ptr shdr) { shaders_.push_back(shdr); }
        size_t shader_count() const { return shaders_.size(); }
        shader_ptr get_shader(size_t idx) const {
            assert(idx < shaders_.size() && ZERR_IDX_OUT_OF_RANGE);
            return shaders_[idx];
        }
        bool link(bool clear=true);

        void bind_uniform(int location, parameter_type type, int count, const char* data);
        template <typename T> void bind_uniform(int location, const T& type);
        template <typename T> void bind_uniform(const char* name, const T& type);
        void bind_texture_unit(int location, int unit);
        void bind_texture_unit(const char* name, int unit);
        void bind_block(int index, int location);

    protected:
        resource_t id_ = 0;
        bool linked_ = false;
        std::vector<shader_ptr> shaders_;
    };

    using program_ptr = std::shared_ptr<program>;

    inline program::program() = default;
    inline program::program(const std::vector<shader_ptr>& shaders) : shaders_(std::move(shaders)) { }
    inline program::program(std::vector<shader_ptr>&& shaders) : shaders_(std::move(shaders)) { }
    inline program::program(const std::string& vshdr, const std::string& fshdr) :
            shaders_{{shader_ptr{new shader{shader_type::ST_VERTEX, vshdr}},
                      shader_ptr{new shader{shader_type::ST_FRAGMENT, fshdr}}}} { }
    inline program::program(program&& rhs) noexcept : id_(rhs.id_), linked_(rhs.linked_), shaders_(std::move(rhs.shaders_)) {
        rhs.id_ = INVALID_RESOURCE; rhs.linked_ = false;
    }
    inline program& program::operator=(program&& rhs) noexcept {
        if(this != &rhs) {
            id_ = rhs.id_; linked_ = rhs.linked_; shaders_ = std::move(rhs.shaders_);
            rhs.id_ = INVALID_RESOURCE; rhs.linked_ = false;
        }
        return *this;
    }

    template <> void program::bind_uniform<int>(int location, const int& value);
    template <> void program::bind_uniform<float>(int location, const float& value);
    template <> void program::bind_uniform<zap::maths::vec2f>(int location, const zap::maths::vec2f& type);
    template <> void program::bind_uniform<zap::maths::vec3f>(int location, const zap::maths::vec3f& type);
    template <> void program::bind_uniform<zap::maths::vec4f>(int location, const zap::maths::vec4f& type);
    template <> void program::bind_uniform<zap::maths::mat3f>(int location, const zap::maths::mat3f& type);
    template <> void program::bind_uniform<zap::maths::mat4f>(int location, const zap::maths::mat4f& type);
    template <> void program::bind_uniform<std::vector<int>>(int location, const std::vector<int>& type);
    template <> void program::bind_uniform<std::vector<zap::maths::vec2f>>(int location, const std::vector<zap::maths::vec2f>& type);
    template <> void program::bind_uniform<std::vector<zap::maths::vec3f>>(int location, const std::vector<zap::maths::vec3f>& type);
    template <> void program::bind_uniform<std::vector<zap::maths::vec4f>>(int location, const std::vector<zap::maths::vec4f>& type);
    template <> void program::bind_uniform<std::vector<float>>(int location, const std::vector<float>& type);

    template <> void program::bind_uniform<int>(const char* name, const int& value);
    template <> void program::bind_uniform<float>(const char* name, const float& value);
    template <> void program::bind_uniform<zap::maths::vec2f>(const char* name, const zap::maths::vec2f& type);
    template <> void program::bind_uniform<zap::maths::vec3f>(const char* name, const zap::maths::vec3f& type);
    template <> void program::bind_uniform<zap::maths::vec4f>(const char* name, const zap::maths::vec4f& type);
    template <> void program::bind_uniform<zap::maths::mat3f>(const char* name, const zap::maths::mat3f& type);
    template <> void program::bind_uniform<zap::maths::mat4f>(const char* name, const zap::maths::mat4f& type);
    template <> void program::bind_uniform<std::vector<int>>(const char* name, const std::vector<int>& type);
    template <> void program::bind_uniform<std::vector<zap::maths::vec2f>>(const char* name, const std::vector<zap::maths::vec2f>& type);
    template <> void program::bind_uniform<std::vector<zap::maths::vec3f>>(const char* name, const std::vector<zap::maths::vec3f>& type);
    template <> void program::bind_uniform<std::vector<zap::maths::vec4f>>(const char* name, const std::vector<zap::maths::vec4f>& type);
    template <> void program::bind_uniform<std::vector<float>>(const char* name, const std::vector<float>& type);

}}

#endif //ZAP_PROGRAM_HPP
