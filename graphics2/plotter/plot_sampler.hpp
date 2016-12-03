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

    template <typename T>
    std::tuple<T, T> find_minmax(const std::vector<T>& data) {
        T min = std::numeric_limits<T>::max(), max = -min;
        for(int i = 0; i != data.size(); ++i) {
            if(data[i] < min) min = data[i];
            if(data[i] > max) max = data[i];
        }
        return std::make_tuple(min, max);
    }

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

        sampler1D(const std::vector<T>& data, interpolator fnc) : data(data), inv_x(1.f/(data.size()-1)), fnc(fnc) {
            std::tie(min, max) = find_minmax(data);
        }

        T operator()(float x) const {
            if(x <  0.f) return data[0];
            if(x >= 1.f) return data.back();

            size_t idx = size_t(x * (data.size()-1));
            float offset = inv_x * idx;
            return (*fnc)(data[idx], data[idx+1], (x - offset)/inv_x);
        }

        std::vector<T> data;
        float inv_x;            // Default to floating point, possibly make configurable
        T min;
        T max;
        interpolator* fnc;
    };


}}

#endif //ZAP_PLOT_SAMPLER_HPP
