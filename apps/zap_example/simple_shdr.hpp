/* Created by Darren Otgaar on 2016/07/31. http://www.github.com/otgaard/zap */
#ifndef ZAP_SIMPLE_SHDR_HPP
#define ZAP_SIMPLE_SHDR_HPP

#include <maths/mat4.hpp>
#include <engine/program.hpp>

// A basic shader for quick use.

class simple_shdr : public zap::engine::program {
public:
    simple_shdr();
    ~simple_shdr();

    bool initialise();

    void pvm_matrix(const zap::maths::mat4f& PVM) { bind(); bind_uniform(pvm_loc_, PVM); release(); }

private:
    int pvm_loc_;
};

#endif //ZAP_SIMPLE_SHDR_HPP
