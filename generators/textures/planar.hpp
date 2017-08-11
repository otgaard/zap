//
// Created by Darren Otgaar on 2016/06/05.
//

#ifndef ZAP_PLANAR_HPP
#define ZAP_PLANAR_HPP

#include <vector>
#include <maths/maths.hpp>

namespace zap { namespace generators {
    template <typename Pixel>
    struct planar {
        template <typename ColourT>
        static std::vector<Pixel> make_checker(size_t width, size_t height, const ColourT& colour_a, const ColourT& colour_b) {
            std::vector<Pixel> image(width*height);

            for(size_t row = 0; row != height; ++row) {
                size_t row_offset = row*width;
                for(size_t col = 0; col != width; ++col) {
                    bool parity = ((row+1)+col)%2 == 0;
                    for(int ch = 0; ch != Pixel::size; ++ch)
                        image[row_offset + col].set(ch, parity ? colour_a[ch] : colour_b[ch]);
                }
            }

            return image;
        }

        template <typename ColourT>
        static std::vector<Pixel> make_fan(size_t width, size_t height, int slices, const ColourT& colour_a, const ColourT& colour_b) {
            std::vector<Pixel> image(width*height);
            const int mod = 360/slices;
            const float inv_w = 1.f/width, inv_h = 1.f/height;
            for(size_t row = 0; row != height; ++row) {
                size_t row_offset = row*width;
                for(size_t col = 0; col != width; ++col) {
                    float x = inv_w*col - .5f; float y = inv_h*row - .5f;
                    bool parity = (int(maths::rad_to_deg(std::atan2(y,x)+maths::PI<float>)) / mod) % 2 == 0;
                    for(int ch = 0; ch != Pixel::size; ++ch)
                        image[row_offset + col].set(ch, parity ? colour_a[ch] : colour_b[ch]);
                }
            }

            return image;
        }

        template <typename ColourT>
        static std::vector<Pixel> make_lerp(size_t width, const ColourT& colour_a, const ColourT& colour_b) {
            std::vector<Pixel> image(width);
            float inv_w = 1.f/width;
            for(size_t col = 0; col != width; ++col) {
                for(int ch = 0; ch != Pixel::size; ++ch) {
                    auto u = col * inv_w;
                    image[col].set(ch, maths::lerp(u, colour_a[ch], colour_b[ch]));
                }
            }

            return image;
        }
    };
}}

#endif //ZAP_PLANAR_HPP
