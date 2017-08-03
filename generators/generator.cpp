/* Created by Darren Otgaar on 2017/06/11. http://www.github.com/otgaard/zap */
#include <maths/rand_lcg.hpp>
#include <tools/threadpool.hpp>
#include "generator.hpp"
#include <maths/simd.hpp>
#include <graphics2/quad.hpp>
#include <maths/io.hpp>
#include <engine/framebuffer.hpp>

using namespace zap;
using namespace maths;
using namespace engine;

const int RND_TBL = 256;
const int RND_MASK = 255;

// Convert cube coordinates to spherical
vec3f cube_to_sphere(const vec3f& P) {
    const float inv3 = 1.f/3.f, x2 = P.x*P.x, y2 = P.y*P.y, z2 = P.z*P.z;
    const float y2z2inv3 = y2*z2*inv3, z2x2inv3 = z2*x2*inv3, x2y2inv3 = x2*y2*inv3;
    return vec3f{
            P.x * sqrtf(1.f - .5f*y2 - .5f*z2 + y2z2inv3),
            P.y * sqrtf(1.f - .5f*z2 - .5f*x2 + z2x2inv3),
            P.z * sqrtf(1.f - .5f*x2 - .5f*y2 + x2y2inv3)
    };
}

const simd::veci RND_MASK_V = simd::load(0xFF);

const char* const value_noise_fshdr = GLSL(
        const int MASK = 0xFF;
        uniform vec4 dims;        // [width, height, inv_w, inv_h]
        uniform usampler1D perm;  // permutation table 256 bytes
        uniform sampler1D grad;   // grad1 table 256 floats 1024 bytes
        in vec2 tex;

        int perm1(int x) { return int(texelFetch(perm, x & MASK, 0).r); }
        int perm2(int x, int y) { return int(texelFetch(perm, (x + perm1(y)) & MASK, 0).r); }
        float grad1(int x) { return texelFetch(grad, perm1(x), 0).r; }
        float grad2(int x, int y) { return texelFetch(grad, perm2(x, y), 0).r; }

        float noise1(float x) {
            float fx = floor(x);
            int ix = int(fx);
            float dx = fx - float(ix);
            return mix(grad1(ix), grad1(ix+1), dx);
        }

        float noise2(float x, float y) {
            float fx = floor(x);
            float fy = floor(y);
            int ix = int(fx);
            int iy = int(fy);
            float dx = x - fx;
            float dy = y - fy;
            return mix(mix(grad2(ix, iy),   grad2(ix+1, iy),   dx),
                       mix(grad2(ix, iy+1), grad2(ix+1, iy+1), dx),
                       dy);
        }

        out float frag_colour;

        void main() {
            frag_colour = noise2(tex.x * dims[2], tex.y * dims[3]);
        }
);

struct generator::state_t {
    bool initialised = false;

    bool initialise(ulonglong seed) {
        zap::maths::rand_lcg prn{seed};
        for(int i = 0; i != RND_TBL; ++i) {
            prn_table[i] = byte(i);
            grad1_table[i] = prn.random_s();
            grad2_table[i] = maths::normalise(maths::vec2f{prn.random_s(), prn.random_s()});
            grad3_table[i] = maths::normalise(maths::vec3f{prn.random_s(), prn.random_s(), prn.random_s()});
        }

        prn.shuffle(prn_table, prn_table+RND_TBL);

        initialised = true;
        return true;
    }

    byte prn_table[RND_TBL] = {};
#if defined(_WIN32)
    __declspec(align(16)) float grad1_table[RND_TBL];
    __declspec(align(16)) maths::vec2f grad2_table[RND_TBL];
#else
    float __attribute__((aligned(16))) grad1_table[RND_TBL] = {};
    maths::vec2f __attribute__((aligned(16))) grad2_table[RND_TBL] = {};
    maths::vec3f __attribute__((aligned(16))) grad3_table[RND_TBL] = {};
#endif

    // OpenGL Resources
    graphics::quad quad;
    texture prn_tex;
    texture grad1_tex;
    framebuffer fbuffer;
    pixel_buffer<r32f_t> pbuffer;

    state_t() = default;

    byte perm(int x) const { return prn_table[x & RND_MASK]; }
    byte perm(int x, int y) const { return perm(x + perm(y)); }
    byte perm(int x, int y, int z) const { return perm(x + perm(y + perm(z))); }
    byte perm(int x, int y, int z, int w) const { return perm(x + perm(y + perm(z + perm(w)))); }
    float grad1(int x) const { return grad1_table[perm(x)]; }
    float grad1(int x, int y) const { return grad1_table[perm(x,y)]; }
    float grad1(int x, int y, int z) const { return grad1_table[perm(x,y,z)]; }
    float grad1(int x, int y, int z, int w) const { return grad1_table[perm(x,y,z,w)]; }
    vec2f grad2(int x, int y) const { return grad2_table[perm(x,y)]; }
    vec3f grad3(int x, int y, int z) const { return grad3_table[perm(x,y,z)]; }
};

generator::generator() : state_(new state_t()), s(*state_.get()) {
}

zap::generator::~generator() = default;

bool zap::generator::initialise(threadpool* pool_ptr, int pool_size, ulonglong seed) {
    if(!pool_ptr) {
        pool_ptr_ = new threadpool{};
        if(!pool_ptr_->initialise(pool_size)) {
            LOG_ERR("Failed to initialise threadpool.  Aborting.");
            return false;
        }
    } else {
        pool_ptr_ = pool_ptr;
    }

    if(!s.initialise(seed)) {
        LOG_ERR("Failed to initialise noise tables");
        return false;
    }

    // Initialise GPU resources
    UNUSED(value_noise_fshdr);
    if(!s.quad.initialise(new shader(shader_type::ST_FRAGMENT, value_noise_fshdr))) {
        LOG_ERR("Failed to initialise render quad");
        return false;
    }

    s.prn_tex.allocate();
    s.prn_tex.initialise(texture_type::TT_TEX1D, RND_TBL, 1, pixel_format::PF_RED_INTEGER, pixel_datatype::PD_DN_UNSIGNED_BYTE, s.prn_table);

    s.grad1_tex.allocate();
    s.grad1_tex.initialise(texture_type::TT_TEX1D, RND_TBL, 1, pixel_format::PF_RED, pixel_datatype::PD_FLOAT, s.grad1_table);

    s.quad.get_program()->bind();
    s.quad.get_program()->bind_texture_unit("perm", 0);
    s.quad.get_program()->bind_texture_unit("grad", 1);
    s.quad.release();

    s.fbuffer.allocate();
    s.pbuffer.allocate();

    return true;
}

std::future<generator::pixmap<float>> generator::render(const render_task& req, generator::gen_method method) {
    if(method == gen_method::GPU) {
        std::promise<generator::pixmap<float>> promise;
        auto fut = promise.get_future();
        promise.set_value(render_gpu(req));
        return fut;
    } else {
        auto fnc = [this, method](render_task r)->generator::pixmap<float>  {
            switch(method) {
                case gen_method::CPU: return this->render_cpu(r);
                case gen_method::SIMD: return this->render_simd(r);
                default: return pixmap<float>{0, 0};    // (due to warning in GCC)
            }
        };

        return pool_ptr_->run_function(fnc, req);
    }
}

pixmap<float> generator::render_cpu(const render_task& req) {
    pixmap<float> image{req.width, req.height, (req.project == render_task::projection::CUBE_MAP ? 6 : 1)};
    LOG("Generating Image:", image.width(), image.height(), image.depth(), image.size());

    if(req.project == render_task::projection::PLANAR) {
        const float inv_x = req.scale.x/req.width;
        const float inv_y = req.scale.y/req.height;

        for(int r = 0; r != req.height; ++r) {
            float y = inv_y * r;
            int iy = maths::floor(y);
            float dy = y - float(iy);
            for(int c = 0; c != req.width; ++c) {
                float x = inv_x * c;
                int ix = maths::floor(x);
                float dx = x - float(ix);
                image(c,r) = pnoise(dx, dy, ix, iy);
            }
        }
    } else if(req.project == render_task::projection::SPHERICAL) {
        const float inv_x = float(TWO_PI)/req.width;
        const float inv_y = float(PI)/req.height;
        const float radius = req.scale.x;

        for(int r = 0; r != req.height; ++r) {
            float theta = inv_y * r, ctheta = cosf(theta), stheta = sinf(theta);
            for(int c = 0; c != req.width; ++c) {
                float phi = inv_x * c, cphi = cosf(phi), sphi = sinf(phi);
                float x = radius * stheta * cphi, y = radius * ctheta, z = radius * stheta * sphi;
                int ix = maths::floor(x), iy = maths::floor(y), iz = maths::floor(z);
                float dx = x - float(ix), dy = y - float(iy), dz = z - float(iz);
                image(c,r) = pnoise(dx, dy, dz, ix, iy, iz);
            }
        }
    } else if(req.project == render_task::projection::CUBE_MAP) {
        assert(req.width == req.height && "Cube Map requires width == height");
        const float inv_dim = 1.f/req.width;
        const int h_dim = req.width/2;

        // X+ Plane
        for(int r = 0; r != req.height; ++r) {
            float y = 2.f * (r - h_dim) * inv_dim;
            for(int c = 0; c != req.width; ++c) {
                float x = 2.f * (h_dim - c) * inv_dim;
                vec3f sP = req.scale.x * cube_to_sphere(vec3f{1.f, y, x});
                int ix = maths::floor(sP.x), iy = maths::floor(sP.y), iz = maths::floor(sP.z);
                float dx = sP.x - float(ix), dy = sP.y - float(iy), dz = sP.z - float(iz);
                image(c,r,0) = pnoise(dx, dy, dz, ix, iy, iz);
            }
        }

        // X- Plane
        for(int r = 0; r != req.height; ++r) {
            float y = 2.f * (r - h_dim) * inv_dim;
            for(int c = 0; c != req.width; ++c) {
                float x = 2.f * (c - h_dim) * inv_dim;
                vec3f sP = req.scale.x * cube_to_sphere(vec3f{-1.f, y, x});
                int ix = maths::floor(sP.x), iy = maths::floor(sP.y), iz = maths::floor(sP.z);
                float dx = sP.x - float(ix), dy = sP.y - float(iy), dz = sP.z - float(iz);
                image(c,r,1) = pnoise(dx, dy, dz, ix, iy, iz);
            }
        }

        // Y+ Plane
        for(int r = 0; r != req.height; ++r) {
            float y = 2.f * (r - h_dim) * inv_dim;
            for(int c = 0; c != req.width; ++c) {
                float x = 2.f * (c - h_dim) * inv_dim;
                vec3f sP = req.scale.x * cube_to_sphere(vec3f{x, -1.f, y});
                int ix = maths::floor(sP.x), iy = maths::floor(sP.y), iz = maths::floor(sP.z);
                float dx = sP.x - float(ix), dy = sP.y - float(iy), dz = sP.z - float(iz);
                image(c,r,2) = pnoise(dx, dy, dz, ix, iy, iz);
            }
        }

        // Y- Plane
        for(int r = 0; r != req.height; ++r) {
            float y = 2.f * (h_dim - r) * inv_dim;
            for(int c = 0; c != req.width; ++c) {
                float x = 2.f * (c - h_dim) * inv_dim;
                vec3f sP = req.scale.x * cube_to_sphere(vec3f{x, 1.f, y});
                int ix = maths::floor(sP.x), iy = maths::floor(sP.y), iz = maths::floor(sP.z);
                float dx = sP.x - float(ix), dy = sP.y - float(iy), dz = sP.z - float(iz);
                image(c,r,3) = pnoise(dx, dy, dz, ix, iy, iz);
            }
        }

        // Z+ Plane
        for(int r = 0; r != req.height; ++r) {
            float y = 2.f * (r - h_dim) * inv_dim;
            for(int c = 0; c != req.width; ++c) {
                float x = 2.f * (c - h_dim) * inv_dim;
                vec3f sP = req.scale.x * cube_to_sphere(vec3f{x, y, 1.f});
                int ix = maths::floor(sP.x), iy = maths::floor(sP.y), iz = maths::floor(sP.z);
                float dx = sP.x - float(ix), dy = sP.y - float(iy), dz = sP.z - float(iz);
                image(c,r,4) = pnoise(dx, dy, dz, ix, iy, iz);
            }
        }

        // Z+ Plane
        for(int r = 0; r != req.height; ++r) {
            float y = 2.f * (r - h_dim) * inv_dim;
            for(int c = 0; c != req.width; ++c) {
                float x = 2.f * (h_dim - c) * inv_dim;
                vec3f sP = req.scale.x * cube_to_sphere(vec3f{x, y, -1.f});
                int ix = maths::floor(sP.x), iy = maths::floor(sP.y), iz = maths::floor(sP.z);
                float dx = sP.x - float(ix), dy = sP.y - float(iy), dz = sP.z - float(iz);
                image(c,r,5) = pnoise(dx, dy, dz, ix, iy, iz);
            }
        }
    }

    return image;
}

pixmap<float> generator::render_simd(const render_task& req) {
    using namespace zap::maths::simd;

    pixmap<float> img{req.width, req.height};

    // Don't support non-aligned sizes for now (need to refactor pixelmap to support row alignment)
    if(req.width % 4 != 0) {
        LOG_ERR("generator::render_simd cannot be used with unaligned memory, width % 4 != 0");
        return img;
    }

    const float inv_x = req.scale.x/req.width;
    const float inv_y = req.scale.y/req.height;

    const int stream_size = 4;
    const int blocks = req.width/stream_size;

    const vecm32f vseq = { 0.f, 1.f, 2.f, 3.f };
    const vecm32f vinc = { inv_x, inv_x, inv_x, inv_x };
    const vecm vsteps = _mm_mul_ps(vseq, vinc);

    set_round_down();

    for(int r = 0; r != req.height; ++r) {
        float vy = r * inv_y;
        int iy0 = zap::maths::floor(vy), iy1 = iy0+1;
        float dy = vy - float(iy0);
        vecm vdy = load(dy);
        int col_offset = r * req.width;
        for(int c = 0, offset = 0; c != blocks; ++c, col_offset += 4, offset += 4) {
            vecm vx = _mm_add_ps(load(offset * inv_x), vsteps);
            vecm fx = ffloor_v(vx);
            veci ix0 = convert_v(fx);
            veci ix1 = _mm_add_epi32(ix0, veci_one);
            vecm vdx = _mm_sub_ps(vx, fx);

            VALIGN int xi0[4], xi1[4];
            _mm_store_si128((veci*)xi0, ix0);
            _mm_store_si128((veci*)xi1, ix1);

            VALIGN float values[16];
            for(int i = 0; i != 4; ++i) {
                values[i]    = s.grad1(xi0[i], iy0);
                values[4+i]  = s.grad1(xi1[i], iy0);
                values[8+i]  = s.grad1(xi0[i], iy1);
                values[12+i] = s.grad1(xi1[i], iy1);
            }

            vecm res = bilinear_v(vdx, vdy,
                                  _mm_load_ps(values),
                                  _mm_load_ps(values+4),
                                  _mm_load_ps(values+8),
                                  _mm_load_ps(values+12));
            _mm_store_ps(img.data(size_t(col_offset)), res);
        }
    }

    set_round_default();

    return img;
}

pixmap<float> generator::render_gpu(const render_task& req) {
    pixmap<float> img{req.width, req.height};

    vec4f dims = {float(req.width), float(req.height), req.scale.x, req.scale.y};

    s.quad.bind();
    s.quad.get_program()->bind_uniform("dims", dims);

    s.prn_tex.bind(0);
    s.grad1_tex.bind(1);

    s.fbuffer.initialise(1, req.width, req.height, pixel_format::PF_RED, pixel_datatype::PD_FLOAT, false, false);

    s.fbuffer.bind();
    s.quad.resize(req.width, req.height);
    s.quad.draw();
    s.fbuffer.release();

    s.grad1_tex.release();
    s.prn_tex.release();

    s.pbuffer.bind();
    s.pbuffer.initialise(req.width, req.height);
    s.pbuffer.release();

    s.fbuffer.read_attachment(s.pbuffer, vec4i{0, 0, req.width, req.height}, 0);

    s.pbuffer.bind(s.pbuffer.read_type);
    if(s.pbuffer.map(buffer_access::BA_READ_ONLY, false)) {
        std::copy((char*)s.pbuffer.data(), (char*)(s.pbuffer.data())+s.pbuffer.size(), (char*)img.data());
        s.pbuffer.unmap(false);
    }
    s.pbuffer.release(s.pbuffer.read_type);

    return img;
}

texture generator::render_texture(const render_task& req, generator::gen_method method) {
    vec4f dims = {float(req.width), float(req.height), req.scale.x, req.scale.y};

    s.quad.bind();
    s.quad.get_program()->bind_uniform("dims", dims);

    s.prn_tex.bind(0);
    s.grad1_tex.bind(1);

    s.fbuffer.initialise(1, req.width, req.height, pixel_format::PF_RED, pixel_datatype::PD_FLOAT, false, false);

    s.fbuffer.bind();
    s.quad.resize(req.width, req.height);
    s.quad.draw();
    s.fbuffer.release();

    s.grad1_tex.release();
    s.prn_tex.release();

    s.pbuffer.bind();
    s.pbuffer.initialise(req.width, req.height);
    s.pbuffer.release();

    s.fbuffer.read_attachment(s.pbuffer, vec4i{0, 0, req.width, req.height}, 0);

    texture tex{};
    tex.allocate();
    tex.initialise(s.pbuffer, false);

    return tex;
}

float generator::vnoise(float dx, int x) const {
    return maths::lerp(dx, s.grad1(x), s.grad1(x+1));
}

float generator::vnoise(float dx, float dy, int x, int y) const {
    int x1 = x+1, y1 = y+1;
    return maths::bilinear(dx, dy, s.grad1(x, y), s.grad1(x1, y), s.grad1(x, y1), s.grad1(x1, y1));
}

float generator::vnoise(float dx, float dy, float dz, int x, int y, int z) const {
    int x1 = x+1, y1 = y+1, z1 = z+1;
    return maths::trilinear(dx, dy, dz, s.grad1(x,y,z),  s.grad1(x1,y,z),  s.grad1(x,y1,z),  s.grad1(x1,y1,z),
                                        s.grad1(x,y,z1), s.grad1(x1,y,z1), s.grad1(x,y1,z1), s.grad1(x1,y1,z1));
}

float generator::pnoise(float dx, int x) const {
    int bx0, bx1;
    float rx0, rx1, sx, u, v;
    bx0 = x;
    bx1 = bx0 + 1;
    rx0 = dx;
    rx1 = rx0 - 1.f;

    sx = easing_curve(rx0);
    u = rx0 * s.grad1(bx0);
    v = rx1 * s.grad1(bx1);
    return lerp(sx, u, v);
}

float generator::pnoise(float dx, float dy, int x, int y) const {
    float sx, sy, a, b, u, v;

    vec2i b0(x, y);
    vec2i b1 = b0 + vec2i{1, 1};
    vec2f r0(dx, dy);
    vec2f r1 = r0 - vec2f{1.f, 1.f};

    sx = easing_curve(r0.x);
    sy = easing_curve(r0.y);

    u = dot(s.grad2(b0.x, b0.y), r0);
    v = dot(s.grad2(b1.x, b0.y), r1.x, r0.y);
    a = lerp(sx, u, v);

    u = dot(s.grad2(b0.x, b1.y), r0.x, r1.y);
    v = dot(s.grad2(b1.x, b1.y), r1);
    b = lerp(sx, u, v);

    return lerp(sy, a, b);
}

float generator::pnoise(float dx, float dy, float dz, int x, int y, int z) const {
    float sx, sy, sz, a, b, c, d, u, v;

    vec3i b0{x, y, z};
    vec3i b1 = b0 + vec3i{1, 1, 1};
    vec3f r0{dx, dy, dz};
    vec3f r1 = r0 - vec3f{1.f, 1.f, 1.f};

    sx = easing_curve(r0.x);
    sy = easing_curve(r0.y);
    sz = easing_curve(r0.z);

    u = dot(s.grad3(b0.x, b0.y, b0.z), r0);
    v = dot(s.grad3(b1.x, b0.y, b0.z), r1.x, r0.y, r0.z);
    a = lerp(sx, u, v);

    u = dot(s.grad3(b0.x, b1.y, b0.z), r0.x, r1.y, r0.z);
    v = dot(s.grad3(b1.x, b1.y, b0.z), r1.x, r1.y, r0.z);
    b = lerp(sx, u, v);

    c = lerp(sy, a, b);

    u = dot(s.grad3(b0.x, b0.y, b1.z), r0.x, r0.y, r1.z);
    v = dot(s.grad3(b1.x, b0.y, b1.z), r1.x, r0.y, r1.z);
    a = lerp(sx, u, v);

    u = dot(s.grad3(b0.x, b1.y, b1.z), r0.x, r1.y, r1.z);
    v = dot(s.grad3(b1.x, b1.y, b1.z), r1);
    b = lerp(sx, u, v);

    d = lerp(sy, a, b);
    return lerp(sz, c, d);
}

