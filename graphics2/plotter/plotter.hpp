/* Created by Darren Otgaar on 2016/11/27. http://www.github.com/otgaard/zap */
#ifndef ZAP_PLOTTER_HPP
#define ZAP_PLOTTER_HPP

#include <memory>
#include <maths/algebra.hpp>
#include <maths/transform.hpp>
#include <renderer/renderer_fwd.hpp>
#include "plot_sampler.hpp"

/*
 * This class is intended for rendering simple plots of piecewise-linearly sampled functions.
 */

namespace zap { namespace graphics {
    using vec2f = maths::vec2f;
    using vec3b = maths::vec3b;

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

        void set_grid(const grid& g);

        void add_plot(const sampler1D<float>& obj, size_t samples, const vec3b& colour);

        bool initialise();

        void update(double t, float dt);
        void draw(const renderer::camera& cam);

        maths::transform3f world_transform;

    protected:
        bool build_grid();

    private:
        struct state_t;
        std::unique_ptr<state_t> state_;
        state_t& s;
    };

}}

#endif //ZAP_PLOTTER_HPP
