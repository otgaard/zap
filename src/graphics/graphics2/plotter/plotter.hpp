/* Created by Darren Otgaar on 2016/11/27. http://www.github.com/otgaard/zap */
#ifndef ZAP_PLOTTER_HPP
#define ZAP_PLOTTER_HPP

#include <memory>
#include <maths/algebra.hpp>
#include <maths/transform.hpp>
#include <renderer/renderer_fwd.hpp>
#include <graphics/graphics2/plotter/plot_sampler.hpp>

/*
 * This class is intended for rendering simple plots of piecewise-linearly sampled functions.
 */

namespace zap { namespace graphics {
    using maths::vec2f;
    using maths::vec3b;

    class plotter {
    public:
        struct grid {
            vec2f domain;           // [ min : max ]
            vec2f domain_maj_min;   // [ major ticks : minor ticks ] or [0,0] for none.
            vec2f range;            // [ min : max ]
            vec2f range_maj_min;    // [ major ticks : minor ticks ]
            bool keep_aspect_ratio; // Keep the aspect ratio (i.e. do not map to UV coords)

            inline void set_domain(float min, float max) { domain.set(min, max); }
            inline void set_range(float min, float max) { range.set(min, max); }
            inline void set_domain_grid(float majors, float minors=0.f) { domain_maj_min.set(majors, minors); }
            inline void set_range_grid(float majors, float minors=0.f) { domain_maj_min.set(majors, minors); }

            grid() = default;
            grid(const vec2f& domain, const vec2f& domain_maj_min, const vec2f& range, const vec2f& range_maj_min,
                 bool keep_aspect_ratio=true) : domain(domain), domain_maj_min(domain_maj_min), range(range),
                                                range_maj_min(range_maj_min), keep_aspect_ratio(keep_aspect_ratio) { }
        };

        plotter();
        plotter(const vec2f& domain, const vec2f& range, const float majors=1.f);
        ~plotter();

        bool is_initialised() const;

        size_t vertex_count() const;
        size_t capacity() const;

        void set_grid(const grid& g);
        void set_reserve(size_t reserve) {
            if(!is_initialised()) {
                reserve_ = reserve;
            }
        }

        template <typename T, typename Interpolator>
        void add_plot(const sampler1D<T, Interpolator>& sampler, size_t samples, const vec3b& colour) {
            build_plot(sampler, vertex_count(), samples, colour);
            add_plot_vertices(samples);
        }

        template <typename T, typename Interpolator>
        void live_plot(const sampler1D<T, Interpolator>& sampler, size_t samples, const vec3b& colour) {
            build_plot(sampler, vertex_count(), samples, colour);
            live_ = true;
            live_samples_ = samples;
        }

        template <typename T, typename PInterpolator, typename CInterpolator>
        void live_plot(const sampler1D<T, PInterpolator>& sampler, const sampler1D<vec3b, CInterpolator>& colour_sampler, size_t samples) {
            build_plot(sampler, colour_sampler, vertex_count(), samples);
            live_ = true;
            live_samples_ = samples;
        }

        bool initialise();

        void update(double t, float dt);
        void draw(const renderer::camera& cam);

        maths::transform3f world_transform;

    protected:
        void add_plot_vertices(size_t samples);

        bool map_buffer();
        void unmap_buffer();
        void set_position(size_t idx, const vec2f& P);
        void set_colour(size_t idx, const vec3b& C);

        bool build_grid();
        template <typename T, typename Interpolator>
        void build_plot(const sampler1D<T, Interpolator>& sampler, size_t start, size_t samples, const vec3b& colour) {
            assert(samples < capacity() && "Out of range error");

            const float inv_x = 1.f/(samples-1);

            if(map_buffer()) {
                for(size_t i = 0; i != samples; ++i) {
                    auto idx = start + i;

                    set_position(idx, vec2f(inv_x * i, sampler(inv_x * i)));
                    set_colour(idx, colour);
                }
                unmap_buffer();
            }
        }

        template <typename T, typename PInterpolator, typename CInterpolator>
        void build_plot(const sampler1D<T, PInterpolator>& sampler, const sampler1D<vec3b, CInterpolator>& colour_sampler, size_t start, size_t samples) {
            assert(samples < capacity() && "Out of range error");

            const float inv_x = 1.f/(samples-1);

            if(map_buffer()) {
                for(size_t i = 0; i != samples; ++i) {
                    auto idx = start + i;

                    const float value = sampler(inv_x * i);
                    set_position(idx, vec2f(inv_x * i, value));
                    // Value can be out of range due to interpolation function (e.g. cubic), therefore clamp
                    set_colour(idx, colour_sampler(zap::maths::clamp(value, 0.f, 1.f)));
                }
                unmap_buffer();
            }
        }

    private:
        size_t reserve_;
        bool live_;
        size_t live_samples_;
        struct state_t;
        std::unique_ptr<state_t> state_;
        state_t& s;
    };

}}

#endif //ZAP_PLOTTER_HPP
