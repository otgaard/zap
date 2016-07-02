//
// Created by Darren Otgaar on 2016/07/02.
//

#include "spectral_analyser.hpp"
#include "fft.hpp"

struct spectral_analyser::state_t {
    fft dft;
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
    a.bin_size = 44100.f/1024.f;    // 43 Hz/bin  Ouch!  Needs to be around 2 Hz per bin to detect around middle C
    a.dominant_frequency = 0.f;     // Filter out 1/f noise and find peak (possibly smoothed) lerp between bins?
    a.estimated_beat = 0.f;         // Lowest 3 or 4 bins, smooth, normalise, and scale to [0..1]
    a.estimated_volume = 0.f;       // bin sum / bins
    return a;
}
