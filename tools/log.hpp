/* Created by Darren Otgaar on 2016/03/05. http://www.github.com/otgaard/zap */
#ifndef ZAP_LOG_HPP
#define ZAP_LOG_HPP

#include <mutex>
#include <memory>
#include <sstream>
#include <iostream>

#include <core/core.hpp>

namespace zap { namespace tools {
    enum class log_level : uint8_t {
        DEBUG,
        WARNING,
        ERROR
    };

    inline void def_cleanup_fnc(std::ostream* stream) { UNUSED(stream); }

    class logger {
    public:
        using cleanup_fnc = std::function<void(std::ostream*)>;
        logger(std::ostream* ostream_ptr, cleanup_fnc fnc=def_cleanup_fnc) : fnc_(fnc), ostream_(ostream_ptr),
                                                                             sstream_("") { }
        virtual ~logger() { if(fnc_) fnc_(ostream_); }

        template<log_level level, typename... Args> void print(Args... args) {
            std::unique_lock<std::mutex> lock(lock_);

            switch(level) {
                case log_level::DEBUG:
                    sstream_ << "<DEBUG>:";
                    break;
                case log_level::ERROR:
                    sstream_ << "<ERROR>:";
                    break;
                case log_level::WARNING:
                    sstream_ << "<WARNING>:";
                    break;
            }
            print_i(args...);
        }

    private:
        void print_i() { (*ostream_) << sstream_.rdbuf() << std::endl; }
        template<typename T, typename... Args> void print_i(T first, Args... rest) {
            sstream_ << first << " ";
            print_i(rest...);
        }

        cleanup_fnc fnc_;
        std::ostream* ostream_;
        std::stringstream sstream_;
        std::mutex lock_;
    };
}}

static zap::tools::logger default_log(&std::cout);

#ifdef LOGGING_ENABLED
#define LOG default_log.print<zap::tools::log_level::DEBUG>
#define LOG_ERR default_log.print<zap::tools::log_level::ERROR>
#define LOG_WARN default_log.print<zap::tools::log_level::WARNING>
#else //LOGGING_ENABLED
#define LOG(...)
#define LOG_ERR(...)
#define LOG_WARN(...)
#endif //LOGGING_ENABLED

#endif //ZAP_LOG_HPP
