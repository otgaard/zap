//
// Created by Darren Otgaar on 2016/03/22.
//

#ifndef ZAP_SHADER_HPP
#define ZAP_SHADER_HPP

#include <string>
#include "engine.hpp"

namespace zap { namespace engine {
    class shader {
    public:
        shader();
        shader(shader_type type, const std::string& src);
        shader(shader&& rhs);
        shader& operator=(shader&& rhs);
        ~shader();

        resource_t resource() const { return id_; }
        void type(shader_type type) { type_ = type; }
        shader_type type() const { return type_; }
        void source(const std::string& src) { src_ = src; }
        const std::string& source() const { return src_; }

        bool is_allocated() const { return id_ != INVALID_RESOURCE; }
        bool is_compiled() const { return is_allocated() && compiled_; }
        resource_t resource_id() const { return id_; }
        bool is_type(shader_type type) const { return type_ == type; }

        bool compile(bool clear=true);

    protected:
        resource_t id_;
        bool compiled_;
        shader_type type_;
        std::string src_;

    private:
        shader(const shader&);
        shader& operator=(const shader&);
    };

    using shader_ptr = std::shared_ptr<shader>;

    inline shader::shader() : id_(INVALID_RESOURCE), compiled_(false) { }
    inline shader::shader(shader_type type, const std::string& src) : id_(INVALID_RESOURCE), compiled_(false),
                                                                      type_(type), src_(src) { }
    inline shader::shader(shader&& rhs) : id_(rhs.id_), compiled_(rhs.compiled_), type_(rhs.type_),
                                          src_(std::move(rhs.src_)) {
        rhs.id_ = INVALID_RESOURCE; rhs.compiled_ = false;
    }

    inline shader& shader::operator=(shader&& rhs) {
        if(this != &rhs) {
            id_ = rhs.id_; compiled_ = rhs.compiled_; type_ = rhs.type_; src_ = std::move(rhs.src_);
            rhs.id_ = INVALID_RESOURCE; rhs.compiled_ = false;
        }
        return *this;
    }
}}

#endif //ZAP_SHADER_HPP
