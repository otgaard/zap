//
// Created by Darren Otgaar on 2016/03/22.
//

#ifndef ZAP_PROGRAM_HPP
#define ZAP_PROGRAM_HPP

#include <vector>
#include "engine.hpp"
#include "shader.hpp"

namespace zap { namespace engine {
    class program {
    public:
        program();
        program(const std::vector<shader_ptr>& shaders);
        program(std::vector<shader_ptr>&& shaders);
        program(program&& rhs);
        program& operator=(program&& rhs);
        ~program();

        std::int32_t uniform_location(const std::string& name);

        inline resource_t resource() const { return id_; }
        inline bool is_allocated() const { return id_ != INVALID_RESOURCE; }
        inline bool is_linked() const { return is_allocated() && linked_; }

        void bind();
        void release();

        inline void add_shader(shader_ptr shdr) { shaders_.push_back(shdr); }
        inline size_t shader_count() const { return shaders_.size(); }
        inline shader_ptr get_shader(size_t idx) const {
            assert(idx < shaders_.size() && ZERR_IDX_OUT_OF_RANGE);
            return shaders_[idx];
        }
        bool link(bool clear=true);

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
}}

#endif //ZAP_PROGRAM_HPP
