//
// Created by Darren Otgaar on 2016/11/27.
//

#ifndef ZAP_PLOT_SAMPLER_HPP
#define ZAP_PLOT_SAMPLER_HPP

/*
 * Defines a sampler to use with the plotter.  The sampler may either be a function or a buffer.
 */

#include <vector>

namespace zap { namespace graphics {
    namespace interpolators {
        template <typename T>
        T nearest(const T& A, const T& B, float parm) {
            return parm < .5f ? A : B;
        }

        template <typename T>
        T linear(const T& A, const T& B, float parm) {
            return (1.f - parm) * A + parm * B;
        }

        template <typename T>
        T cubic(const T& A, const T& B, float parm) {
            float val = parm * parm * (3.f - 2 * parm);
            return (1.f - val) * A + val * B;
        }
    }

    template <typename T>
    struct sampler1D {
        using interpolator = decltype(interpolators::nearest<T>);

        sampler1D() : fnc(nullptr) { }
        sampler1D(const std::vector<T>& data, interpolator fnc) : data(data), inv_u(1.f/(data.size()-1)), fnc(fnc) {
        }

        T operator()(float u) const {
            assert(0 <= u && u <= 1 && "Sampler parameter out of range [0, 1]");
            size_t idx = size_t(u * (data.size()-1));
            float offset = inv_u * idx;
            return (*fnc)(data[idx], data[idx+1], (u - offset)/inv_u);
        }

        std::vector<T> data;
        float inv_u;            // Default to floating point, possibly make configurable
        interpolator* fnc;
    };


}}

#endif //ZAP_PLOT_SAMPLER_HPP
