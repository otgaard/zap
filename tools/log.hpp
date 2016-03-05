/* Created by Darren Otgaar on 2016/03/05. http://www.github.com/otgaard/zap */
#ifndef ZAP_LOG_HPP
#define ZAP_LOG_HPP

#include <mutex>
#include <memory>
#include <iosfwd>
#include <sstream>

namespace zap { namespace tools {
    enum class log_level : char {
        DEBUG,
        WARNING,
        ERROR
    };

    class log {
    public:
        template<log_level level, typename... Args> void print(Args... args);

    private:
        template<typename T, typename... Args> void print(T car, Args... cons);
    };
}}

#endif //ZAP_LOG_HPP
