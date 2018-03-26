//
// Created by Darren Otgaar on 2017/06/10.
//

#ifndef ZAP_THREADPOOL_HPP
#define ZAP_THREADPOOL_HPP

#include <asio.hpp>
#include <core/core.hpp>
#include <thread>
#include <future>
#include <tuple>
#include <vector>
#include <functional>

namespace zap {

class threadpool {
public:
    threadpool() : work_(io_service) { }
    threadpool(const threadpool&) = delete;
    ~threadpool() {
        if(!io_service.stopped()) io_service.stop();
        for(auto& thr : threads_) thr.join();
    }
    threadpool& operator=(const threadpool&) = delete;

    bool initialise(int thread_count=2) {
        threads_.reserve(thread_count);
        for(auto i = 0; i != thread_count; ++i) {
            threads_.emplace_back(std::thread([this](){ io_service.run(); }));
        }
        return true;
    }

    // The arguments to the functions are packed into a copied tuple to prevent temporary destruction.  Remember to
    // explicitly pass references using std::ref or std::cref.

    template <typename Fnc, typename... Args>
    void run_task(Fnc&& fnc, Args&&... arguments) {
        auto parms = std::make_tuple(std::forward<Args>(arguments)...);
        auto new_fnc = [](Fnc&& fnc, std::tuple<Args...> args) { apply(std::forward<Fnc>(fnc), args); };
        io_service.post(std::bind(std::forward<Fnc>(fnc), parms));
    }

    // Wraps a function in a future and executes it in the threadpool
#ifndef _WIN32
    template <typename Fnc, typename... Args>
    auto run_function(Fnc&& fnc, Args&&... arguments) -> std::future<decltype(fnc(arguments...))> {
        using RetT = decltype(fnc(arguments...));
        auto parms = std::make_tuple(std::forward<Args>(arguments)...);
        auto promise = std::make_shared<std::promise<RetT>>();
        auto fut = promise->get_future();

        auto new_fnc = [promise](Fnc&& fnc, std::tuple<Args...> args) {
            promise->set_value(apply(std::forward<Fnc>(fnc), args));
        };

        io_service.post(std::bind(new_fnc, std::forward<Fnc>(fnc), parms));

        return fut;
    }
#else
    template <typename Fnc, typename... Args>
    auto run_function(Fnc&& fnc, Args&&... arguments) -> std::future<decltype(fnc(arguments...))> {
        using RetT = decltype(fnc(arguments...));
        auto parms = std::make_tuple(std::forward<Args>(arguments)...);
        auto promise = std::make_shared<std::promise<RetT>>();
        auto fut = promise->get_future();

        auto new_fnc = [args{std::move(parms)}, promise](Fnc&& fnc) {
            promise->set_value(apply(std::forward<Fnc>(fnc), args));
        };

        io_service.post(std::bind(new_fnc, std::forward<Fnc>(fnc)));

        return fut;
    }
#endif

    asio::io_service io_service;

private:
    asio::io_service::work work_;
    std::vector<std::thread> threads_;
};

/*
template <typename Fnc, typename... Args, typename RetT = typename std::result_of<Fnc(Args...)>::type>
RetT caller(Fnc&& fnc, Args&&... args) {
    return std::forward<Fnc>(fnc)(std::forward<Args>(args)...);
}

template <typename Fnc, typename... Args, typename RetT = typename std::result_of<Fnc(Args...)>::type>
RetT binder(Fnc&& fnc, Args&&... args) {
    auto parms = std::tuple<Args...>(std::forward<Args>(args)...);
    auto new_fnc = [parms](Fnc&& f) {
        return apply(f, parms);
    };

    auto callable = std::bind(new_fnc, std::forward<Fnc>(fnc));
    return callable();
}
*/

}

#endif //ZAP_THREADPOOL_HPP
