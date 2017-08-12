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

inline maths::vec3f cube_to_sphere(const maths::vec3f& P) {
    const float inv3 = 1.f/3.f, x2 = P.x*P.x, y2 = P.y*P.y, z2 = P.z*P.z;
    const float y2z2inv3 = y2*z2*inv3, z2x2inv3 = z2*x2*inv3, x2y2inv3 = x2*y2*inv3;
    return maths::vec3f{
            P.x * sqrtf(1.f - .5f*y2 - .5f*z2 + y2z2inv3),
            P.y * sqrtf(1.f - .5f*z2 - .5f*x2 + z2x2inv3),
            P.z * sqrtf(1.f - .5f*x2 - .5f*y2 + x2y2inv3)
    };
}

struct render_task {
    using vec2f = maths::vec2f;
    using mat2f = maths::mat2f;

    enum class basis_function {
        VALUE,
        PERLIN,
        SIMPLEX,
        WORLEY,
        WHITE,
        USER_FUNCTION
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
    bool mipmaps = false;

    render_task(int width, int height, basis_function bf=basis_function::VALUE) :
            basis_fnc{bf},
            width{width},
            height{height} {
    }
};

class generator {
public:
    using texture = zap::engine::texture;
    template <typename PixelT> using pixmap = engine::pixmap<PixelT>;
    template <typename PixelT> using pixmap_future = std::future<pixmap<PixelT>>;

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

    // This is a temporary interface for sharing the PRN tables with other shaders via the loaded textures
    const texture* prn_table() const;
    const texture* grad1_table() const;

    // Creates a R32F floating point texture i.e. pixmap<float>
    texture render_texture(const render_task& req, gen_method method=gen_method::GPU);

    // Note:
    // These functions require the following signatures:
    // Planar: PixelT fnc(float x, float y, generator& gen);
    // Spherical: PixelT fnc(float x, float y, float z, generator& gen);
    // Cube Map: PixelT fnc(float x, float y, float z, generator& gen);

    //template <typename Fnc> texture render(const render_task& req, Fnc&& fnc);
    template <typename Fnc> texture render_planar(const render_task& req, Fnc&& fnc);
    template <typename Fnc> texture render_spherical(const render_task& req, Fnc&& fnc);
    template <typename Fnc> texture render_cylindrical(const render_task& req, Fnc&& fnc);
    template <typename Fnc> texture render_cubemap(const render_task& req, Fnc&& fnc);

    template <typename PixelT>
    pixmap_future<PixelT> render_image(const render_task& req, gen_method method=gen_method::CPU);
    // Render the resource for usage on the client
    pixmap_future<float> render(const render_task& req, gen_method method=gen_method::CPU);

    pixmap<float> render_cpu(const render_task& req);
    pixmap<float> render_simd(const render_task& req);
    pixmap<float> render_gpu(const render_task& req);

    float vnoise(float dx, int x) const;
    float vnoise(float dx, float dy, int x, int y) const;
    float vnoise(float dx, float dy, float dz, int x, int y, int z) const;
    float pnoise(float dx, int x) const;
    float pnoise(float dx, float dy, int x, int y) const;
    float pnoise(float dx, float dy, float dz, int x, int y, int z) const;

    inline float easing_curve(float value) const { return value * value * (3.f - 2.f * value); }

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

/*
template <typename Fnc>
engine::texture generator::render(const render_task& req, Fnc&& fnc) {
    switch(req.project) {
        case render_task::projection::PLANAR: return render_planar(req, fnc);
        case render_task::projection::SPHERICAL: return render_spherical(req, fnc);
        case render_task::projection::CUBE_MAP: return render_cubemap(req, fnc);
        default: return texture{};
    }
}
*/
template <typename Fnc>
engine::texture generator::render_planar(const render_task& req, Fnc&& fnc) {
    const float inv_x = req.scale.x/req.width;
    const float inv_y = req.scale.y/req.height;

    using PixelT = typename std::result_of<Fnc(float, float, generator&)>::type;
    pixmap<PixelT> image{req.width, req.height};

    for(int r = 0; r != req.height; ++r) {
        float y = inv_y * r;
        for(int c = 0; c != req.width; ++c) {
            float x = inv_x * c;
            image(c,r) = fnc(x, y, *this);
        }
    }

    texture tex{};
    if(!tex.allocate() || !tex.initialise(image, req.mipmaps)) LOG_ERR("Failed to create planar texture");
    return tex;
}

template <typename Fnc>
engine::texture generator::render_spherical(const render_task& req, Fnc&& fnc) {
    const float inv_x = maths::TWO_PI<float>/(req.width-1);
    const float inv_y = maths::PI<float>/(req.height-1);
    const float radius = req.scale.x;

    using PixelT = typename std::result_of<Fnc(float, float, float, generator&)>::type;
    pixmap<PixelT> image{req.width, req.height};

    for(int r = 0; r != req.height; ++r) {
        float theta = inv_y * r, ctheta = cosf(theta), stheta = sinf(theta);
        for(int c = 0; c != req.width; ++c) {
            float phi = inv_x * c, cphi = cosf(phi), sphi = sinf(phi);
            float x = radius * stheta * cphi, y = radius * ctheta, z = radius * stheta * sphi;
            image(c,r) = fnc(x, y, z, *this);
        }
    }

    texture tex{};
    if(!tex.allocate() || !tex.initialise(image, req.mipmaps)) LOG_ERR("Failed to create spherical texture");
    return tex;
}

template <typename Fnc>
engine::texture generator::render_cylindrical(const render_task& req, Fnc&& fnc){
    const float inv_x = maths::TWO_PI<float>/(req.width-1);
    const float inv_y = req.scale.y/(req.height-1);
    const float radius = req.scale.x;

    using PixelT = typename std::result_of<Fnc(float, float, float, generator&)>::type;
    pixmap<PixelT> image{req.width, req.height};

    for(int r = 0; r != req.height; ++r) {
        float y = inv_y * r;
        for(int c = 0; c != req.width; ++c) {
            float theta = inv_x * c;
            float x = radius * cosf(theta), z = radius * sinf(theta);
            image(c,r) = fnc(x, y, z, *this);
        }
    }

    texture tex{};
    if(!tex.allocate() || !tex.initialise(image, req.mipmaps)) LOG_ERR("Failed to create cylindrical texture");
    return tex;
}

template <typename Fnc>
engine::texture generator::render_cubemap(const render_task& req, Fnc&& fnc) {
    using vec3f = maths::vec3f;
    assert(req.width == req.height && "Cube Map requires width == height");
    const float inv_dim = 1.f/req.width;
    const int h_dim = req.width/2;

    using PixelT = typename std::result_of<Fnc(float, float, float, generator&)>::type;
    pixmap<PixelT> image{req.width, req.height, 6};

    // X+ Plane
    for(int r = 0; r != req.height; ++r) {
        float y = 2.f * (r - h_dim) * inv_dim;
        for(int c = 0; c != req.width; ++c) {
            float x = 2.f * (h_dim - c) * inv_dim;
            vec3f sP = req.scale.x * cube_to_sphere(vec3f{1.f, y, x});
            image(c,r,0) = fnc(sP.x, sP.y, sP.z, *this);
        }
    }

    // X- Plane
    for(int r = 0; r != req.height; ++r) {
        float y = 2.f * (r - h_dim) * inv_dim;
        for(int c = 0; c != req.width; ++c) {
            float x = 2.f * (c - h_dim) * inv_dim;
            vec3f sP = req.scale.x * cube_to_sphere(vec3f{-1.f, y, x});
            image(c,r,1) = fnc(sP.x, sP.y, sP.z, *this);
        }
    }

    // Y+ Plane
    for(int r = 0; r != req.height; ++r) {
        float y = 2.f * (r - h_dim) * inv_dim;
        for(int c = 0; c != req.width; ++c) {
            float x = 2.f * (c - h_dim) * inv_dim;
            vec3f sP = req.scale.x * cube_to_sphere(vec3f{x, -1.f, y});
            image(c,r,2) = fnc(sP.x, sP.y, sP.z, *this);
        }
    }

    // Y- Plane
    for(int r = 0; r != req.height; ++r) {
        float y = 2.f * (h_dim - r) * inv_dim;
        for(int c = 0; c != req.width; ++c) {
            float x = 2.f * (c - h_dim) * inv_dim;
            vec3f sP = req.scale.x * cube_to_sphere(vec3f{x, 1.f, y});
            image(c,r,3) = fnc(sP.x, sP.y, sP.z, *this);
        }
    }

    // Z+ Plane
    for(int r = 0; r != req.height; ++r) {
        float y = 2.f * (r - h_dim) * inv_dim;
        for(int c = 0; c != req.width; ++c) {
            float x = 2.f * (c - h_dim) * inv_dim;
            vec3f sP = req.scale.x * cube_to_sphere(vec3f{x, y, 1.f});
            image(c,r,4) = fnc(sP.x, sP.y, sP.z, *this);
        }
    }

    // Z- Plane
    for(int r = 0; r != req.height; ++r) {
        float y = 2.f * (r - h_dim) * inv_dim;
        for(int c = 0; c != req.width; ++c) {
            float x = 2.f * (h_dim - c) * inv_dim;
            vec3f sP = req.scale.x * cube_to_sphere(vec3f{x, y, -1.f});
            image(c,r,5) = fnc(sP.x, sP.y, sP.z, *this);
        }
    }

    texture tex{engine::texture_type::TT_CUBE_MAP};
    if(!tex.allocate() || !tex.initialise(image, req.mipmaps)) LOG_ERR("Failed to create cube_map texture");
    return tex;
}


}
#endif //ZAP_GENERATOR_HPP
