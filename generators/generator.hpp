/* Created by Darren Otgaar on 2017/06/11. http://www.github.com/otgaard/zap */
#ifndef ZAP_GENERATOR_HPP
#define ZAP_GENERATOR_HPP

/* The generator is an interface to the CPU/GPU routines necessary to create textures and other images from the GPU
 * or on the CPU.  The goal is to provide an interface that allows seamless generation of noise on the CPU or GPU ensuring
 * that the results are consistent between processor types.  The interface is future-based and therefore preferably
 * async.
 */

#include <memory>
#include <future>
#include <maths/vec2.hpp>
#include <maths/vec3.hpp>
#include <maths/mat2.hpp>
#include <maths/mat3.hpp>
#include <engine/pixmap.hpp>

namespace zap {

struct render_task {
    using vec2f = maths::vec2f;
    using vec3f = maths::vec3f;
    using mat2f = maths::mat2f;
    using mat3f = maths::mat3f;

    enum class basis_function {
        LINEAR_VALUE,
        CUBIC_VALUE,
        PERLIN,
        SIMPLEX,
        WORLEY,
        WHITE
    } basis_fnc;

    int width, height;

    vec2f translation;
    mat2f rotation;
    vec2f scale;
    bool is_unit[3];

    render_task(int width, int height, basis_function bf=basis_function::LINEAR_VALUE) :
            basis_fnc{bf},
            width{width},
            height{height},
            translation{0.f, 0.f},
            rotation{1.f, 1.f},
            scale{1.f, 1.f},
            is_unit{true, true, true} {
    }
};

// Starting with Linear Value Noise

class generator {
public:
    template <typename PixelT>
    using pixmap = engine::pixmap<PixelT>;
    template <typename PixelT>
    using pixmap_future = std::future<pixmap<PixelT>>;

    enum class gen_method {
        CPU,
        SIMD,
        GPU
    };

    generator();
    ~generator();

    bool initialise(threadpool* pool_ptr=nullptr, int pool_size=2, ulonglong seed=1);

    pixmap_future<float> render(const render_task& req, gen_method method=gen_method::CPU);

protected:
    pixmap<float> render_cpu(const render_task& req);
    pixmap<float> render_simd(const render_task& req);
    pixmap<float> render_gpu(const render_task& req);

private:
    struct state_t;
    std::unique_ptr<state_t> state_;
    state_t& s;
};

}
#endif //ZAP_GENERATOR_HPP
