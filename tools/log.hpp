/* Created by Darren Otgaar on 2016/03/05. http://www.github.com/otgaard/zap */
#ifndef ZAP_LOG_HPP
#define ZAP_LOG_HPP

#include <mutex>
#include <memory>
#include <sstream>
#include <iostream>

#include <core/core.hpp>

#if !defined(_WIN32)
const char* const LOG_RESET = "\033[0m";
const char* const LOG_BLACK = "\033[30m";
const char* const LOG_RED = "\033[31m";
const char* const LOG_GREEN = "\033[32m";
const char* const LOG_YELLOW = "\033[33m";
const char* const LOG_BLUE = "\033[34m";
const char* const LOG_MAGENTA = "\033[35m";
const char* const LOG_CYAN = "\033[36m";
const char* const LOG_WHITE = "\033[37m";
const char* const LOG_BOLDBLACK = "\033[1m\033[30m";
const char* const LOG_BOLDRED = "\033[1m\033[31m";
const char* const LOG_BOLDGREEN = "\033[1m\033[32m";
const char* const LOG_BOLDYELLOW = "\033[1m\033[33m";
const char* const LOG_BOLDBLUE = "\033[1m\033[34m";
const char* const LOG_BOLDMAGENTA = "\033[1m\033[35m";
const char* const LOG_BOLDCYAN = "\033[1m\033[36m";
const char* const LOG_BOLDWHITE = "\033[1m\033[37m";
#else
const char* const LOG_RESET = "";
const char* const LOG_BLACK = "";
const char* const LOG_RED = "";
const char* const LOG_GREEN = "";
const char* const LOG_YELLOW = "";
const char* const LOG_BLUE = "";
const char* const LOG_MAGENTA = "";
const char* const LOG_CYAN = "";
const char* const LOG_WHITE = "";
const char* const LOG_BOLDBLACK = "";
const char* const LOG_BOLDRED = "";
const char* const LOG_BOLDGREEN = "";
const char* const LOG_BOLDYELLOW = "";
const char* const LOG_BOLDBLUE = "";
const char* const LOG_BOLDMAGENTA = "";
const char* const LOG_BOLDCYAN = "";
const char* const LOG_BOLDWHITE = "";
#endif

namespace zap { namespace tools {
    enum class log_level : uint8_t {
        LL_DEBUG,
        LL_WARNING,
        LL_ERROR
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
                case log_level::LL_DEBUG:
                    sstream_ << "<DEBUG>:";
                    break;
                case log_level::LL_ERROR:
                    sstream_ << "<ERROR>:" << LOG_BOLDRED;
                    break;
                case log_level::LL_WARNING:
                    sstream_ << "<WARNING>:"  << LOG_BOLDYELLOW;
                    break;
            }

            print_i(args...);
        }

    private:
        void print_i() { (*ostream_) << sstream_.rdbuf() << LOG_RESET << std::endl; }
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

#if defined(LOGGING_ENABLED)
#define LOG default_log.print<zap::tools::log_level::LL_DEBUG>
#define LOG_ERR default_log.print<zap::tools::log_level::LL_ERROR>
#define LOG_WARN default_log.print<zap::tools::log_level::LL_WARNING>
#else //!LOGGING_ENABLED
#define LOG(...)
//#define LOG_ERR(...)
#define LOG_ERR default_log.print<zap::tools::log_level::LL_ERROR>
#define LOG_WARN(...)
#endif //!LOGGING_ENABLED

#endif //ZAP_LOG_HPP
