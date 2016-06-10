/* Created by Darren Otgaar on 2016/06/08. http://www.github.com/otgaard/zap */
#ifndef ZAP_FFT_HPP
#define ZAP_FFT_HPP

#include <vector>
#include <maths/maths.hpp>

class fft {
public:
    typedef std::vector<float> buffer_t;
    fft(size_t frame_size=512) : frame_size_(frame_size), prev_(2*frame_size_, 0.0f), curr_(2*frame_size, 0.0f),
        smoothing_(5*frame_size, 0.0f) { }

    void run_fft(const std::vector<float>& samples, std::vector<float>& bins, size_t count=512);
    void process_samples(const float* buffer, size_t channels, buffer_t& result);
    void fourier_transform(buffer_t& fft_buffer, size_t window, bool inverse=false);

protected:
    inline float hamming_window(size_t n, size_t N) {
        return 0.54f - 0.46f * std::sinf(2.0f * (float)zap::maths::TWO_PI * n)/(N - 1);
    }

private:
    size_t frame_size_;
    buffer_t prev_;
    buffer_t curr_;
    buffer_t smoothing_;
};

#endif //ZAP_FFT_HPP
