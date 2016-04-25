//
// Created by Darren Otgaar on 2016/04/17.
//

#define LOGGING_ENABLED

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <tools/log.hpp>
#include <vector>
#include <portaudio.h>
#include <numeric>
#include <cmath>
#include <maths/maths.hpp>
#include <base/mp3_stream.hpp>
#include <thread>
#include <engine/shader.hpp>
#include <engine/program.hpp>
#include <maths/vec2.hpp>
#include <maths/vec3.hpp>
#include <engine/buffer_format.hpp>
#include <maths/functions.hpp>
#include "engine/vertex_buffer.hpp"
#include <engine/gl_api.hpp>
#include <engine/types.hpp>

// An MP3 player using the Fast Fourier transform and wavelets for spectral analysis and synchronised procedural
// generation. Essentially a fun app to write in order to motivate adding the various parts of the engine.

std::mutex track_mux;
std::vector<float> track_sample_buffer(100);
size_t sample_count = 0;

constexpr float inv_s16 = 1.f/std::numeric_limits<short>::max();

using unique_lock = std::unique_lock<std::mutex>;

const char* mp3file = "/Users/otgaard/Development/zap/output/assets/aphextwins.mp3";

typedef int callback(const void* input, void* output, u_long frame_count,
                     const PaStreamCallbackTimeInfo* time_info, PaStreamCallbackFlags status_flags,
                     void* userdata);

int mp3_callback(const void* input, void* output, u_long frames, const PaStreamCallbackTimeInfo* time_info,
        PaStreamCallbackFlags status_flags, void* userdata) {
    static std::vector<short> buffer;
    mp3_stream* stream = static_cast<mp3_stream*>(userdata);
    const auto& mp3 = stream->get_header();
    short* out = static_cast<short*>(output);

    const size_t req_frames = mp3.channels*frames;

    if(buffer.size() != req_frames) buffer.resize(req_frames);
    auto len = stream->read(buffer, req_frames);
    if(len == 0) { LOG("Complete"); return paComplete; }

    // Summarise the stream for display
    {
        unique_lock lock(track_mux);
        if(sample_count < track_sample_buffer.size()-1) {
            float sample = 0;
            auto skip = mp3.channels == 2 ? 2 : 1;
            for(auto i = 0; i < req_frames; i += skip) {
                sample += buffer[i] * inv_s16;
            }
            track_sample_buffer[sample_count] = sample/(frames/skip);
            sample_count++;
        }
    }

    std::copy(buffer.begin(), buffer.begin()+len, out);
    return paContinue;
}

void play_aphextwins() {
    mp3_stream mstream(mp3file, 1024, nullptr);
    mstream.start();
    if(!mstream.is_open()) {
        LOG_ERR("Error opening MP3 File:", mp3file);
        return;
    }

    if(Pa_Initialize() != paNoError) {
        LOG_ERR("Error initialising portaudio");
        return;
    }

    PaStreamParameters outputParameters;
    outputParameters.device = Pa_GetDefaultOutputDevice();
    if(outputParameters.device == paNoDevice) {
        LOG_ERR("No suitable output device found by portaudio");
        return;
    }

    LOG(mstream.get_header().channels, mstream.get_header().samplerate, mstream.get_header().bitrate);

    PaStream* stream;
    PaError err = Pa_OpenDefaultStream(
            &stream,
            0,
            mstream.get_header().channels,
            paInt16,
            mstream.get_header().samplerate,
            mstream.get_header().channels == 1 ? 1024 : 512,
            &mp3_callback,
            &mstream
    );

    if(err != paNoError) {
        LOG_ERR("Pa_OpenStream failed:", err, Pa_GetErrorText(err));
        if(stream) Pa_CloseStream(stream);
        return;
    }

    if(Pa_StartStream(stream) != paNoError) {
        LOG_ERR("Pa_OpenStream failed:", err, Pa_GetErrorText(err));
        if(stream) Pa_CloseStream(stream);
        return;
    }

    while(Pa_IsStreamActive(stream) > 0) Pa_Sleep(1000);

    if(Pa_Terminate() != paNoError) {
        LOG_ERR("Pa_Terminate error:", err, Pa_GetErrorText(err));
        return;
    }
}

static void on_error(int error, const char* description) {
    LOG_ERR("GLFW Error:", error, "Description:", description);
}

#define GLSL(src) "#version 330 core\n" #src

const char* vtx_src = GLSL(
        uniform mat4 proj_matrix;
        in vec2 position;
        out vec2 st;

        void main() {
            st = vec2(position.x/20.0 + 0.5, position.y/2.0 + 0.5);
            gl_Position = proj_matrix*vec4(position.x, position.y, 0.0, 1.0);
        }
);

const char* frg_src = GLSL(
        out vec4 frag_colour;
        in vec2 st;

        void main() {
            frag_colour = vec4(st.x, st.y, 1.0 - st.x, 1.0);
        }
);

using namespace zap::maths;
using namespace zap::engine;

int main(int argc, char* argv[]) {
    glfwSetErrorCallback(::on_error);

    if(!glfwInit()) return -1;

    glfwWindowHint(GLFW_SAMPLES, 8);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    auto window = glfwCreateWindow(1280, 768, "synesthesia", nullptr, nullptr);
    if(!window) {
        LOG_ERR("Error creating window - terminating");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if(err != GLEW_OK) {
        LOG("GLEW failed to initialise: ", glewGetErrorString(err));
        return -1;
    }
    LOG("Suppressing error generated by GLEW", glGetError());

    glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
    glfwSwapInterval(1);

    glViewport(0, 0, 1280, 768);

    std::vector<shader_ptr> arr;
    arr.push_back(std::make_shared<shader>(shader_type::ST_VERTEX, vtx_src));
    arr.push_back(std::make_shared<shader>(shader_type::ST_FRAGMENT, frg_src));

    auto prog = std::make_shared<program>(std::move(arr));
    prog->link(true);
    gl_error_check();

    zap::maths::mat4<float> proj_matrix = {
            100/1280.f,       0.f, 0.f, 0.f,
            0.f,        200/768.f, 0.f, 0.f,
            0.f,              0.f, 2.f, 0.f,
            0.f,              0.f, 0.f, 1.f
    };

    prog->bind();
    auto loc = prog->uniform_location("proj_matrix");
    glUniformMatrix4fv(loc, 1, GL_FALSE, proj_matrix.data());
    prog->release();
    gl_error_check();

    vertex_buffer<pos3_t, buffer_usage::BU_STATIC_DRAW> frame;
    vertex_buffer<pos3_t, buffer_usage::BU_DYNAMIC_DRAW> graph;

    std::vector<pos3_t> box;
    box.push_back(pos3_t({{-10,-1,0}}));
    box.push_back(pos3_t({{10,-1,0}}));
    box.push_back(pos3_t({{10,1,0}}));
    box.push_back(pos3_t({{-10,1,0}}));

    GLuint frame_mesh;
    glGenVertexArrays(1, &frame_mesh);
    glBindVertexArray(frame_mesh);
    frame.allocate();
    frame.bind();
    frame.initialise(box);
    glBindVertexArray(0);
    gl_error_check();

    GLuint line_mesh;
    glGenVertexArrays(1, &line_mesh);
    glBindVertexArray(line_mesh);
    graph.allocate();
    graph.bind();
    graph.initialise(1000);
    graph.map(buffer_access::BA_WRITE_ONLY);
    for(int i = 0; i < 1000; ++i) {
        (graph.begin()+i)->position.x = -10.f + i*2/100.f;
        (graph.begin()+i)->position.y = 0.f;
        (graph.begin()+i)->position.z = 0.f;
    }
    graph.unmap();
    glBindVertexArray(0);
    gl_error_check();

    std::thread my_thread(play_aphextwins);

    while(!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        prog->bind();

        glBindVertexArray(frame_mesh);
        glDrawArrays(GL_LINE_LOOP, 0, frame.vertex_count());
        glBindVertexArray(0);

        graph.bind();
        graph.map(buffer_access::BA_READ_WRITE);
        {
            std::unique_lock<std::mutex> scoped_lock(track_mux);
            if(sample_count != 0) {
                for(auto it = graph.end() - sample_count - 1, end = graph.begin() - sample_count - 1; it != end; --it) {
                    it->position.y = (it - sample_count)->position.y;
                }

                for(auto it = track_sample_buffer.begin(),
                         end = track_sample_buffer.begin() + sample_count; it != end; ++it) {
                    (graph.begin() + (it - track_sample_buffer.begin()))->position.y = (*it);
                }

                sample_count = 0;
            }
        }
        graph.unmap();
        graph.release();

        glBindVertexArray(line_mesh);
        glDrawArrays(GL_LINE_STRIP, 0, graph.vertex_count());
        glBindVertexArray(0);

        prog->release();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    my_thread.join();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
