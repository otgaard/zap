/* Created by Darren Otgaar on 2016/10/12. http://www.github.com/otgaard/zap */
#ifndef ZAP_STYLE_HPP
#define ZAP_STYLE_HPP

#include <engine/program.hpp>

namespace zap { namespace engine { namespace renderer {
    class style {
    public:
        style() = default;
        style(size_t passes) : pass_count_(passes), passes_(passes, nullptr) { }

        size_t pass_count() const { return pass_count_; }

        void pass_program(size_t idx, program* prog_ptr) { assert(idx < pass_count_); passes_[idx] = prog_ptr; }
        program* pass_program(size_t idx) { assert(idx < pass_count_); return passes_[idx]; }
        const program* pass_program(size_t idx) const { assert(idx < pass_count_); return passes_[idx]; }



    protected:
        size_t pass_count_;
        std::vector<program*> passes_;
    };
}}}

#endif //ZAP_STYLE_HPP
