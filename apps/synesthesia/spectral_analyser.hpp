//
// Created by Darren Otgaar on 2016/07/02.
//

#ifndef ZAP_SPECTRAL_ANALYSER_HPP
#define ZAP_SPECTRAL_ANALYSER_HPP

/* The spectral analyser examines the FFT data and produces a variety of outputs based on
 * the current FFT.  The analyser will perform smoothing, pitch analysis, beat analysis, and
 * attempt to build heuristics for synchronising with audio content.
 */

#include <memory>
#include <vector>

struct analysis {
    float dominant_frequency;
    float bin_size;
    float estimated_volume;
    float estimated_beat;       // a linear interpolation between 0 no beat and 1 centre of beat curve
};

class spectral_analyser {
public:
    using fft_window = std::array<float, 64>;

    spectral_analyser();
    ~spectral_analyser();

    analysis process_samples(const std::vector<float>& samples, fft_window& window);

protected:


private:
    struct state_t;
    std::unique_ptr<state_t> state_;
};

#endif //ZAP_SPECTRAL_ANALYSER_HPP
