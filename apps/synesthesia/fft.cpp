/* Created by Darren Otgaar on 2016/06/08. http://www.github.com/otgaard/zap */
#include "fft.hpp"
#define LOGGING_ENABLED
#include <tools/log.hpp>

// From implementation described at:
// http://blogs.zynaptiq.com/bernsee/dft-a-pied/

// Triangular smoothing function
constexpr static float tri_smooth[5] = { 1.f, 2.f, 3.f, 2.f, 1.f };
constexpr static float inv_tri = 1.f/9.f; // or 1/5 for box smoothing && { 1, 1, 1, 1, 1 };

void fft::run_fft(const std::vector<float>& samples, std::vector<float>& bins, size_t count) {
    if(samples.size() < count) return;

    const static float inv_s =1.f/count;

    if(bins.size() != samples.size()) bins.resize(samples.size());
    process_samples(samples.data(), 2, bins);

    for(int i = 0; i != 512; ++i) {
        const auto idx = 2*i;
        float mag = 20.f * std::log10f(2.f * inv_s * std::sqrt(bins[idx]*bins[idx] + bins[idx+1]*bins[idx+1]));
        for(int k = 4; k != 0; --k) smoothing_[5*i+k] = smoothing_[5*i+(k-1)];
        smoothing_[5*i] = mag; mag = 0;
        for(int k = 0; k != 5; ++k) mag += tri_smooth[k]*smoothing_[5*i+k];
        mag *= inv_tri;
        bins[i] = (zap::maths::clamp(mag, -100.f, 0.f) + 100.f)*0.01f;
    }
}

void fft::process_samples(const float* buffer, size_t channels, buffer_t& result) {
    assert(channels == 2 && "FFT expects two channels, interleave upstream!");

    const size_t samples = frame_size_*2;
    const size_t fft_window = 2 * samples;
    if(result.size() < fft_window) result.resize(fft_window);

    for(size_t i = 0; i < samples; ++i) {
        const auto idx = 2*i;
        if(i < frame_size_) {
            result[idx]     = prev_[idx] * hamming_window(i, samples);
            result[idx + 1] = 0;    // Left channel only, use complex part for 2-channel interleaved
        } else {
            const auto offset = idx - samples;
            result[idx]     = buffer[offset] * hamming_window(i, samples);
            result[idx + 1] = 0;
            curr_[offset] = buffer[offset];
        }
    }

    fourier_transform(result, samples, false);
    prev_ = curr_;
}

void fft::fourier_transform(buffer_t& fft_buffer, size_t window, bool inverse) {
    int sign = inverse ? -1 : 1;
    float wr, wi, arg, temp;
    int p1, p2;
    float tr, ti, ur, ui;
    int p1r, p1i, p2r, p2i;
    int i, bitm, j, le, le2, k;
    size_t fftFrameSize2 = window * 2;

    for(i = 2; i < fftFrameSize2 - 2; i += 2) {
        for(bitm = 2, j = 0; bitm < fftFrameSize2; bitm <<= 1) {
            if((i & bitm) != 0) j++;
            j <<= 1;
        }
        if(i < j) {
            p1 = i; p2 = j;
            temp = fft_buffer[p1];
            fft_buffer[p1++] = fft_buffer[p2];
            fft_buffer[p2++] = temp;
            temp = fft_buffer[p1];
            fft_buffer[p1] = fft_buffer[p2];
            fft_buffer[p2] = temp;
        }
    }

    int kmax = (int)(std::logf(window) / std::logf(2.0) + 0.5);
    for (k = 0, le = 2; k < kmax; k++) {
        le <<= 1;
        le2 = le >> 1;
        ur = 1.0f;
        ui = 0.0f;
        arg = (float)(zap::maths::PI / (le2 >> 1));
        wr = std::cosf(arg);
        wi = sign * std::sinf(arg);
        for (j = 0; j < le2; j += 2) {
            p1r = j; p1i = p1r + 1;
            p2r = p1r + le2; p2i = p2r + 1;
            for (i = j; i < fftFrameSize2; i += le) {
                float p2rVal = fft_buffer[p2r];
                float p2iVal = fft_buffer[p2i];
                tr = p2rVal * ur - p2iVal * ui;
                ti = p2rVal * ui + p2iVal * ur;
                fft_buffer[p2r] = fft_buffer[p1r] - tr;
                fft_buffer[p2i] = fft_buffer[p1i] - ti;
                fft_buffer[p1r] += tr;
                fft_buffer[p1i] += ti;
                p1r += le;
                p1i += le;
                p2r += le;
                p2i += le;
            }
            tr = ur * wr - ui * wi;
            ui = ur * wi + ui * wr;
            ur = tr;
        }
    }
}
