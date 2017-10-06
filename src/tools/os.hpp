//
// Created by Darren Otgaar on 2016/10/28.
//

#ifndef ZAP_OS_HPP
#define ZAP_OS_HPP

#include <string>
#include <vector>

namespace zap {
    bool is_file(const std::string& filename);
    bool is_dir(const std::string& path);
    std::vector<std::string> get_files(const std::string& path);
};

#endif //ZAP_OS_HPP
