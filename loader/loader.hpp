/* Created by Darren Otgaar on 2017/04/08. http://www.github.com/otgaard/zap */
#ifndef ZAP_LOADER_HPP
#define ZAP_LOADER_HPP

// Asset loader (models, images, etc)

#include <string>
#include <vector>

class loader {
public:
    loader();
    ~loader();

    void load(const std::string& path);

private:
    struct state_t;
    std::unique_ptr<state_t> state_;
    state_t& s;
};


#endif //ZAP_LOADER_HPP
