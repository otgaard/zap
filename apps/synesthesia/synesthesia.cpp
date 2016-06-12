//
// Created by Darren Otgaar on 2016/06/10.
//

#include <atomic>
#include <unordered_map>

#include <portaudio.h>

#define LOGGING_ENABLED
#include <tools/log.hpp>
#include <maths/maths.hpp>
#include <engine/engine.hpp>
#include <engine/program.hpp>
#include <base/mp3_stream.hpp>

/****************
 * LOCAL INCLUDES
 ****************/
#include "fft.hpp"
#include "application.hpp"
#include "modules/module.hpp"
#include "modules/bars.hpp"
#include "modules/metaballs.hpp"

using namespace zap;
using namespace zap::maths;
using namespace zap::engine;

/**********************
 * CONSTANT DEFINITIONS
 **********************/

constexpr float inv_s16 = 1.f/std::numeric_limits<short>::max();
const char* mp3file = "/Users/otgaard/synesthesia_music_folder/3.mp3";
//const char* mp3file = "/Users/otgaard/Development/zap/output/assets/chembros4.mp3";
//const char* mp3file = "/Users/otgaard/test.mp3";

/*************
 * STREAM_DATA
 *************/

struct stream_data {
    std::unique_ptr<mp3_stream> stream;
    std::atomic<bool> read_flag;
    std::atomic<bool> write_flag;
    module::fft_analysis_t curr_analysis;
    fft fourier;
} stream;

/**************
 * MP3_CALLBACK
 **************/

int mp3_callback(const void* input, void* output, u_long frames, const PaStreamCallbackTimeInfo* time_info,
                 PaStreamCallbackFlags status_flags, void* userdata) {
    static std::vector<short> buffer(1024, 0);
    static std::vector<float> samples(1024, 0.f);
    static std::vector<float> analysis(1024, 0.f);

    // This delay syncs the FFT with the audible frame
    static std::vector<short> delay(1024, 0);
    static size_t delay_count = 0;
    static size_t output_len = 0;

    stream_data* stream_ptr = static_cast<stream_data*>(userdata);
    const auto& mp3 = stream_ptr->stream->get_header();
    short* out = static_cast<short*>(output);

    const size_t req_frames = mp3.channels*frames;
    if(buffer.size() != req_frames) {
        buffer.resize(req_frames);
        samples.resize(req_frames);
        analysis.resize(req_frames);
        delay.resize(req_frames);
    }

    buffer = delay;
    output_len = delay_count;
    delay_count = stream_ptr->stream->read(delay, req_frames);
    if(delay_count == 0 && output_len == 0) { return paComplete; }

    if(delay_count > 0) {
        std::transform(delay.begin(), delay.begin() + delay_count, samples.begin(), [](short s) { return inv_s16 * s; });
        stream_ptr->fourier.run_fft(samples, analysis);
        if(stream_ptr->write_flag) {
            std::copy(analysis.begin(), analysis.begin() + stream_ptr->curr_analysis.size(), stream_ptr->curr_analysis.begin());
            stream_ptr->write_flag = false;
            stream_ptr->read_flag = true;
        }
    }

    std::copy(buffer.begin(), buffer.begin()+output_len, out);
    return paContinue;
}

enum class audio_state {
    AS_STOPPED,
    AS_PLAYING,
    AS_PAUSED
};

class synesthesia : public application {
public:
    synesthesia() : application("synesthesia"), state_(audio_state::AS_STOPPED), pa_stream_(nullptr) { }
    void initialise() final;
    void update(double t, float dt) final;
    void draw() final;
    void shutdown() final;

    void play();
    void pause();
    void stop();

    inline bool is_playing() const { return state_ == audio_state::AS_PLAYING; }
    inline bool is_paused() const { return state_ == audio_state::AS_PAUSED; }
    inline bool is_stopped() const { return state_ == audio_state::AS_STOPPED; }

private:
    bool play_mp3(const std::string& path);

    audio_state state_;
    PaStream* pa_stream_;
    std::vector<std::unique_ptr<module>> modules_;

    std::unique_ptr<stream_data> stream_ptr_;
};

/*********************
 * SYNESTHESIA METHODS
 *********************/

void synesthesia::play() {
    if(is_stopped() && !stream_ptr_) {
        stream_ptr_ = std::make_unique<stream_data>();
        stream_ptr_->stream = std::make_unique<mp3_stream>(mp3file, 4 * 1024, nullptr);
        state_ = audio_state::AS_PLAYING;
        play_mp3(mp3file);
    } else if(is_paused()) {
        state_ = audio_state::AS_PLAYING;
    }
}

void synesthesia::pause() {
    state_ = audio_state::AS_PAUSED;
}

void synesthesia::stop() {
    state_ = audio_state::AS_STOPPED;
}

void synesthesia::initialise() {
    // Initialise state...
    bf_culling(true);

    // Initialise bars...
    modules_.emplace_back(std::make_unique<bars>(this));
    modules_.emplace_back(std::make_unique<metaballs>(this));

    play();
}

/**********
 * PLAY_MP3
 **********/

bool synesthesia::play_mp3(const std::string& path) {
    if(!stream_ptr_->stream->start() || !stream_ptr_->stream->is_open()) { LOG_ERR("Error opening MP3 File:", path); return false; }
    if(Pa_Initialize() != paNoError) { LOG_ERR("Error initialising portaudio"); return false; }

    PaStreamParameters outputParameters;
    outputParameters.device = Pa_GetDefaultOutputDevice();
    if(outputParameters.device == paNoDevice) { LOG_ERR("No suitable output device found by portaudio"); return false; }

    LOG(stream_ptr_->stream->get_header().channels, stream_ptr_->stream->get_header().samplerate, stream_ptr_->stream->get_header().bitrate);

    stream_ptr_->write_flag = true;

    //PaStream* stream;
    PaError err = Pa_OpenDefaultStream(
            &pa_stream_,
            0,
            stream_ptr_->stream->get_header().channels,
            paInt16,
            stream_ptr_->stream->get_header().samplerate,
            stream_ptr_->stream->get_header().channels == 1 ? 1024 : 512,
            &mp3_callback,
            stream_ptr_.get()
    );

    if(err != paNoError) {
        LOG_ERR("Pa_OpenStream failed:", err, Pa_GetErrorText(err));
        if(pa_stream_) Pa_CloseStream(pa_stream_);
        return false;
    }

    if(Pa_StartStream(pa_stream_) != paNoError) {
        LOG_ERR("Pa_OpenStream failed:", err, Pa_GetErrorText(err));
        if(pa_stream_) Pa_CloseStream(pa_stream_);
        return false;
    }

    return true;
}

/********
 * UPDATE
 ********/

void synesthesia::update(double t, float dt) {
    if(pa_stream_ && Pa_IsStreamActive(pa_stream_) <= 0) {
        if(Pa_Terminate() != paNoError) LOG_ERR("Pa_Terminate error");
        LOG("Finished mp3");
        stream_ptr_.reset(nullptr);
        pa_stream_ = nullptr;

        // Zero the fft
        module::fft_analysis_t zeros;
        for(auto& v : zeros) v = -0.5f;
        for(auto& mod : modules_) mod->set_analysis(zeros);
    }

    if(stream_ptr_ && stream_ptr_->read_flag) {
        for(auto& mod : modules_) mod->set_analysis(stream_ptr_->curr_analysis);
        stream_ptr_->read_flag = false;
        stream_ptr_->write_flag = true;
    }

    for(auto& mod : modules_) mod->update(t, dt);
}

/******
 * DRAW
 ******/

void synesthesia::draw() {
    for(auto& mod : modules_) mod->draw();
}

void synesthesia::shutdown() {

}

int main(int argc, char* argv[]) {
    synesthesia app;
    return app.run();
}