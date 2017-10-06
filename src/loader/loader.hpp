/* Created by Darren Otgaar on 2017/04/08. http://www.github.com/otgaard/zap */
#ifndef ZAP_LOADER_HPP
#define ZAP_LOADER_HPP

// Asset loader (models, images, etc)

#include <memory>
#include <string>
#include <vector>

#include "graphics3/g3_types.hpp"

class loader {
public:
    using mesh_p3n3t2_trii_t = zap::graphics::mesh_p3n3t2_trii_t;

    loader();
    ~loader();

    std::vector<mesh_p3n3t2_trii_t> load(const std::string& path);

private:
    struct state_t;
    std::unique_ptr<state_t> state_;
    state_t& s;
};

#endif //ZAP_LOADER_HPP
