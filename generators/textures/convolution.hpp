//
// Created by Darren Otgaar on 2016/06/22.
//

#ifndef ZAP_CONVOLUTION_HPP
#define ZAP_CONVOLUTION_HPP

#include <maths/maths.hpp>

namespace zap { namespace generators {
    template <typename PixelT>
    struct convolution {
        static std::vector<PixelT> boxblur_vert(size_t width, size_t height, size_t radius, std::vector<PixelT>& image) {
            std::vector<PixelT> output(image.size());
            const float iarr = 1.f/(radius+radius+1);
            for(size_t i = 0; i != height; ++i) {
                auto ti = i*width, li = ti, ri = ti+radius;
                auto fv = image[ti].template get3v<float>(), lv = image[ti+width-1].template get3v<float>(), val = (radius+1)*fv;
                for(size_t j = 0; j != radius; ++j) { val += image[ti+j].template get3v<float>(); }
                for(size_t j = 0; j <= radius; ++j) { val += image[ri++].template get3v<float>() - fv; output[ti++].set3(val*iarr); }
                for(size_t j = radius+1; j != width - radius; ++j) { val += image[ri++].template get3v<float>() - image[li++].template get3v<float>(); output[ti++].set3(val*iarr); }
                for(size_t j = width-radius; j != width; ++j) { val += lv - image[li++].template get3v<float>(); output[ti++].set3(val*iarr); }
            }
            return output;
        }

        static std::vector<PixelT> boxblur_horz(size_t width, size_t height, size_t radius, std::vector<PixelT>& image) {
            std::vector<PixelT> output(image.size());
            const float iarr = 1.f/(radius+radius+1);
            for(size_t i = 0; i != width; ++i) {
                auto ti = i, li = ti, ri = ti+radius*width;
                auto fv = image[ti].template get3v<float>(), lv = image[ti+width*(height-1)].template get3v<float>(), val = (radius+1)*fv;
                for(size_t j = 0; j != radius; ++j) { val += image[ti+j*width].template get3v<float>(); }
                for(size_t j = 0; j <= radius; ++j) { val += image[ri].template get3v<float>() - fv; output[ti].set3(val*iarr); ri += width; ti += width;  }
                for(size_t j = radius+1; j != height - radius; ++j) { val += image[ri].template get3v<float>() - image[li].template get3v<float>(); output[ti].set3(val*iarr); li += width; ri += width; ti += width; }
                for(size_t j = height-radius; j != height; ++j) { val += lv - image[li].template get3v<float>(); output[ti].set3(val*iarr); li += width; ti += width; }
            }
            return output;
        }

        static std::vector<PixelT> boxblur(size_t width, size_t height, size_t radius, std::vector<PixelT>& image) {
            auto hb = boxblur_horz(width, height, radius, image);
            return boxblur_vert(width, height, radius, hb);
        }
    };
}}

#endif //ZAP_CONVOLUTION_HPP
