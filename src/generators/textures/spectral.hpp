//
// Created by Darren Otgaar on 2016/05/16.
//

#ifndef ZAP_SPECTRAL_HPP
#define ZAP_SPECTRAL_HPP

#include <vector>
#include <maths/maths.hpp>

// Provides spectral synthesis tools for generating textures on the CPU.

namespace zap { namespace generators {
    template <typename Pixel>
    struct spectral {
        static std::vector<Pixel> make_clouds(int terms, float scale, size_t width, size_t height) {
            float i, amplitude, f;
            float x, fx, xfreq, xphase;
            float y, fy, yfreq, yphase;
            const float offset = 0.5;
            const float xoffset = 13;
            const float yoffset = 96;

            std::vector<float> sample(width*height);
            std::vector<Pixel> image(width*height);

            const float dx = scale/width;
            const float dy = scale/height;

            constexpr float TWO_PI = 2.f*float(zap::maths::PI);
            constexpr float HALF_PI = .5f*float(zap::maths::PI);
            constexpr float xfreq_origin = TWO_PI * .023f;
            constexpr float yfreq_origin = TWO_PI * .021f;

            for(size_t row = 0; row != height; ++row) {
                y = scale * (row * dy + yoffset);
                for(size_t col = 0; col != width; ++col) {
                    x = scale * (col * dx + xoffset);

                    xphase = .9f; yphase = .7f;
                    xfreq = xfreq_origin; yfreq = yfreq_origin;
                    amplitude = .3f;
                    f = 0;

                    for(i = 0; i < terms; ++i) {
                        fx = wave_fnc<float, decltype(std::cosf)>(std::cosf, x, xfreq, amplitude, xphase, offset);
                        fy = wave_fnc<float, decltype(std::cosf)>(std::cosf, y, yfreq, amplitude, yphase, offset);
                        f += fx + fy;
                        xphase = HALF_PI*.9f * std::cos(yfreq * y);
                        yphase = HALF_PI*1.1f * std::cos(xfreq * x);

                        xfreq *= 1.9f + .1f*i;
                        yfreq *= 2.2f - .08f*i;
                        amplitude *= .707f;
                    }

                    sample[row*width + col] = zap::maths::clamp(f);
                }
            }

            // Convert to target pixel
            const typename Pixel::type type_max = std::numeric_limits<typename Pixel::type>::max();
            //const float type_scale = 1.f / type_max;
            for(size_t idx = 0; idx != image.size(); ++idx) {
                auto val = typename Pixel::type(type_max*sample[idx]);
                for(int j = 0; j < Pixel::data_t::size; ++j) image[idx].set(j, val);
            }

            return image;
        }

        template <typename T, typename FNC>
        inline static T wave_fnc(FNC fnc, T x, T frequency=T(1), T amplitude=T(1), T phase=(0), T offset=T(0)) {
            return amplitude * (offset + fnc(frequency * (x + phase)));
        }

        // Higher order function to create a wave function from an input periodic function with range [-1, 1]
        // fnc - a periodic function (e.g. sin, cos, sawtooth, square wave, etc.)
        template <typename FNC>
        struct wave {
            constexpr static auto make_fnc(FNC fnc, float frequency, float amplitude, float phase, float offset) {
                return [=](float x) -> float {
                    return amplitude * (offset + fnc(frequency * (x + phase)));
                };
            };
        };
    };
}}

#endif //ZAP_SPECTRAL_HPP
