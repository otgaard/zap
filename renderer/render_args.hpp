//
// Created by Darren Otgaar on 2017/08/18.
//

#ifndef ZAP_RENDER_ARGS_HPP
#define ZAP_RENDER_ARGS_HPP

/**
 * render_args provide the method by which render_contexts are re-used.  The render_args are constructed as a structure
 * that overrides the default render_context binding (the master render_context).  The render_args are a block of memory
 * that can be passed to the render_context when drawing an instance so that:
 *
 * 1) All transformations for the target mesh are updated and available.
 * 2) All non-default parameters are bound before drawing.
 * 3) All non-default render_state is configured before drawing.
 * 4) All non-default state is restored after drawing (optional).
 */

#include <functional>
#include <renderer/render_context.hpp>

namespace zap { namespace renderer {
    struct argument {
        enum class arg_type {
            AT_CONSTANT,        // Overrides the default but stays the same across repeated calls
            AT_VARIABLE,        // Is changed per frame or per draw
            AT_FUNCTION         // Is evaluated each call
        } type;

        using update_fnc = std::function<void(char*)>; // TODO: Try to remove indirection with predefined fncs

        int idx;                // Parameter index (in render_context::parameters_ not OpenGL location and not arguments_)
        int offset;
        update_fnc fnc;

        argument(arg_type at, int idx, int offset) : type(at), idx(idx), offset(offset) { }
        argument(arg_type at, int idx, int offset, update_fnc&& fnc) : type(at), idx(idx), offset(offset), fnc(std::move(fnc)) { }
    };

    class render_args {
    public:
        explicit render_args(render_context* context) : context_(context) { }
        render_args(const render_args& rhs) : context_(rhs.context_), arg_buffer_(rhs.arg_buffer_) { }
        render_args(render_args&& rhs) noexcept : context_(rhs.context_), arg_buffer_(std::move(rhs.arg_buffer_)) { }
        ~render_args() = default;

        render_args& operator=(const render_args& rhs) {
            if(this != &rhs) {
                context_ = rhs.context_;
                arg_buffer_ = rhs.arg_buffer_;
            }
            return *this;
        }

        render_args& operator=(render_args&& rhs) noexcept {
            if(this != &rhs) {
                context_ = rhs.context_;
                arg_buffer_ = std::move(rhs.arg_buffer_);
            }
            return *this;
        }

        template <typename T>
        void set_parameter(int idx, const T& value) {
            int arg_idx = find_argument(idx);
            if(arg_idx == -1) {
                auto parm = context_->get_parameter(idx);
                assert(parm.bytesize() == sizeof(T) && "Mismatched parameters");
                if(parm.bytesize() != sizeof(T)) return;

                arg_idx = arguments_.size();
                arguments_.emplace_back(argument::arg_type::AT_CONSTANT, idx, buf_size_);
                buf_size_ += parm.bytesize();
                arg_buffer_.resize(buf_size_);
            }
            memcpy(arg_buffer_.begin()+arguments_[arg_idx].offset, value, sizeof(T));
        }

        template <typename T>
        void set_parameter(const std::string& name, const T& value) {
            auto idx = context_->get_index(name);
            set_parameter(idx, value);
        }

    private:
        int find_argument(int parm_idx) const {
            auto it = std::find_if(arguments_.begin(), arguments_.end(), [&parm_idx](const auto& arg) {
                return parm_idx == arg.idx;
            });
            return it != arguments_.end() ? int(it - arguments_.begin()) : -1;
        }

        bool argument_exists(int parm_idx) const {
            return find_argument(parm_idx) != -1;
        }

        render_context* context_ = nullptr;
        int buf_size_ = 0;
        std::vector<argument> arguments_ = {};  // The overridden arguments
        std::vector<char> arg_buffer_ = {};     // Temporary, move to custom allocator
    };
}}

#endif //ZAP_RENDER_ARGS_HPP
