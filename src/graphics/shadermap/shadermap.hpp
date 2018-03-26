//
// Created by otgaard on 2018/03/14.
//

#ifndef ZAP_SHADERMAP_HPP
#define ZAP_SHADERMAP_HPP

#include <string>
#include <unordered_map>
#include <engine/program.hpp>

namespace zap { namespace graphics {

// Define a program for compilation
struct program_def {
    std::string name;
    std::string vshdr;
    std::string gshdr;
    std::string fshdr;

    program_def(std::string&& name, std::string&& vshdr, std::string&& fshdr)
            : name(std::move(name)), vshdr(std::move(vshdr)), fshdr(std::move(fshdr)) { }
};

class shadermap {
public:
    using program = engine::program;

protected:

private:
    std::unordered_map<std::string, std::unique_ptr<program>> map_;
};

}}

#endif //ZAP_SHADERMAP_HPP
