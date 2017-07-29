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
#include <maths/mat2.hpp>
#include <engine/pixmap.hpp>
#include <engine/texture.hpp>
#include <tools/threadpool.hpp>
#include "pixel_conversion.hpp"

namespace zap {

struct render_task {
    using vec2f = maths::vec2f;
    using mat2f = maths::mat2f;

    enum class basis_function {
        VALUE,
        PERLIN,
        SIMPLEX,
        WORLEY,
        WHITE
    } basis_fnc;

    enum class interpolation {
        LINEAR,
        CUBIC,
        QUINTIC
    } interp = interpolation::LINEAR;

    enum class projection {
        PLANAR,
        SPHERICAL,
        CYLINDRICAL,
        TOROIDAL,
        CUBE_MAP        // Returns a 6-layered pixmap containing [x_min, x_max, y_min, y_max, z_min, z_max]
    } project = projection::PLANAR;

    int width, height;

    vec2f translation = {0.f, 0.f};
    mat2f rotation = {1.f, 0.f,
                      0.f, 1.f};
    vec2f scale = {1.f, 1.f};
    bool is_unit[3] = {true, true, true};

    render_task(int width, int height, basis_function bf=basis_function::VALUE) :
            basis_fnc{bf},
            width{width},
            height{height} {
    }
};

// Starting with Linear Value Noise
class generator {
public:
    using texture = zap::engine::texture;
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
    generator(const generator&) = delete;
    generator(generator&&) = delete;
    ~generator();

    generator& operator=(const generator&) = delete;
    generator& operator=(generator&&) = delete;

    bool initialise(threadpool* pool_ptr=nullptr, int pool_size=2, ulonglong seed=1);

    // Creates a R32F floating point texture i.e. pixmap<float>
    texture render_texture(const render_task& req, gen_method method=gen_method::GPU);

    template <typename PixelT>
    pixmap_future<PixelT> render_image(const render_task& req, gen_method method=gen_method::CPU);
    // Render the resource for usage on the client
    pixmap_future<float> render(const render_task& req, gen_method method=gen_method::CPU);

    pixmap<float> render_cpu(const render_task& req);
    pixmap<float> render_simd(const render_task& req);
    pixmap<float> render_gpu(const render_task& req);

protected:
    float vnoise(float dx, int x) const;
    float vnoise(float dx, float dy, int x, int y) const;
    float vnoise(float dx, float dy, float dz, int x, int y, int z) const;

private:
    threadpool* pool_ptr_ = nullptr;

    struct state_t;
    std::unique_ptr<state_t> state_;
    state_t& s;
};

template <typename PixelT>
generator::pixmap_future<PixelT> generator::render_image(const render_task& req, generator::gen_method method) {
    auto fnc = [this, method](render_task r)->generator::pixmap<PixelT> {
        auto input = render(r, method).get();
        pixmap<PixelT> img{r.width, r.height, (r.project == render_task::projection::CUBE_MAP ? 6 : 1)};
        convert(input, img);
        return img;
    };

    return pool_ptr_->run_function(fnc, req);
}

}
#endif //ZAP_GENERATOR_HPP
