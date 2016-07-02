//
// Created by Darren Otgaar on 2016/07/02.
//

#include "spectral_analyser.hpp"
#include "fft.hpp"
#include <apps/graphic_types.hpp>
#include <maths/functions.hpp>

struct spectral_analyser::state_t {
    fft dft;

    timer time;

    struct beat_data {
        constexpr static size_t kernel_size = 5;
        constexpr static size_t window_size = 4;
        std::vector<float> window;
        bool started;
        float start_t;
        int counter;
        std::vector<float> history; // Store a history of the value over the last 10 samples
        beat_data() : window(window_size*kernel_size, 0), started(false), start_t(0), counter(0), history(10, 0) { }
    } beat;
};

spectral_analyser::spectral_analyser() : state_(std::make_unique<spectral_analyser::state_t>()) {
}

spectral_analyser::~spectral_analyser() {
}

analysis spectral_analyser::process_samples(const std::vector<float>& samples, fft_window& window) {
    std::vector<float> buffer(1024, 0);
    state_->dft.run_fft(samples, buffer);
    std::copy(buffer.begin(), buffer.begin()+window.size(), window.begin());
    analysis a;

    // Need a bin size of approximately 15 Hz to detect pitch changes around middle C.  This means we would need
    // to collect a minimum of 8192 samples giving:
    // 8192 / 2 = 4096 bins                  Nyquist Frequency
    // 44100 / 4096 = 10.7 Hz / bin
    // and the time resolution is
    // 4096/44100 = 93 milliseconds

    // Current fft analysis covers a window of 1024
    // i.e 512 bins
    // 44100 / 512 = 86 Hz / bin
    // 512 / 44100 = 0.011s or slightly less than a frame at 60 Hz

    a.bin_size = 44100.f/512.f;     // 86 Hz/bin  Ouch!  Needs to be around 15 Hz per bin to detect around middle C
    a.dominant_frequency = 0.f;     // lerp between bins?
    a.estimated_beat = beat_analysis(window);         // Lowest 3 or 4 bins, smooth, normalise, and scale to [0..1]
    a.estimated_volume = 0.f;       // bin sum / bins
    return a;
}

float spectral_analyser::beat_analysis(const fft_window& window) {
    //static const float smoothing_term[5] = { 1.f, 2.f, 3.f, 2.f, 1.f };
    //static const float inv_scale = 1.f/9.f;

    //constexpr static size_t kernel_size = state_t::beat_data::kernel_size;
    //constexpr static size_t window_size = state_t::beat_data::window_size;
    //constexpr static float inv_winsize = 1.f/window_size;

    //auto& beat = state_->beat;
    return (window[0] + window[1] + window[2] + window[3])/4.f;


    /* Triangle Convolution
    float mag[window_size];
    for(auto& r : mag) r = 0.f;

    for(size_t i = kernel_size; i != 0; --i) {        // Samples
        for(size_t b = 0; b != window_size; ++b) {    // Bin
            beat.window[window_size*i+b] = beat.window[window_size*(i-1)+b];
            mag[b] += smoothing_term[i] * beat.window[window_size*i+b];
        }
    }

    for(size_t i = 0; i != window_size; ++i) {
        beat.window[i] = window[i];
        mag[i] += smoothing_term[i] * window[i];
    }

    float total = 0.f;
    for(size_t i = 0; i != window_size; ++i) total += inv_scale*mag[i];
    return clamp(total * inv_winsize);
    */
}

float spectral_analyser::dominant_frequency(const fft_window& window) {
    return 0;
}

float spectral_analyser::volume_analysis(const fft_window& window) {
    return 0;
}
