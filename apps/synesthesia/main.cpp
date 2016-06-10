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
#include <engine/vertex_format.hpp>
#include <maths/functions.hpp>
#include "engine/vertex_buffer.hpp"
#include <engine/gl_api.hpp>
#include <engine/types.hpp>
#include <engine/mesh.hpp>
#include "fft.hpp"
#include <dirent.h>
#include <sys/stat.h>
#include "shader_src.hpp"
#include <engine/uniform_buffer.hpp>
#include <engine/texture.hpp>
#include <generators/textures/planar.hpp>
#include <generators/geometry/surface.hpp>

void get_files(const std::string& path, std::vector<std::string>& out) {
#ifdef _WIN32
    HANDLE dir;
    WIN32_FIND_DATA file_data;

    if((dir = FindFirstFile((directory + "/*").c_str(), &file_data)) == INVALID_HANDLE_VALUE) return;

    do {
        const string file_name = directory + "/" + file_data.cFileName;
        if(file_data.cFileName[0] == '.' || (file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) continue;
        out.push_back(file_name);
    } while(FindNextFile(dir, &file_data));

    FindClose(dir);
#else
    DIR* dir;
    struct dirent* ent;
    struct stat st;

    dir = opendir(path.c_str());
    while((ent = readdir(dir)) != nullptr) {
        const std::string file_path = path + "/" + ent->d_name;
        if(ent->d_name[0] == '.' || stat(file_path.c_str(), &st) == -1 || (st.st_mode & S_IFDIR) != 0) continue;
        out.push_back(file_path);
    }
    closedir(dir);
#endif
}

// An MP3 player using the Fast Fourier transform and wavelets for spectral analysis and synchronised procedural
// generation. Essentially a fun app to write in order to motivate adding the various parts of the engine.

static std::mutex track_mux;
constexpr size_t sample_buffer_size = 100;
std::vector<float> track_sample_buffer(sample_buffer_size);
size_t sample_count = 0;
std::atomic<bool> terminate(false);
bool next_track = false;
std::vector<float> fft_samples(1024);
bool new_samples = false;

// Timer for testing
/*
zap::maths::timer t;
int last = 0;
*/

constexpr float inv_s16 = 1.f/std::numeric_limits<short>::max();

using unique_lock = std::unique_lock<std::mutex>;

typedef int callback(const void* input, void* output, u_long frame_count,
                     const PaStreamCallbackTimeInfo* time_info, PaStreamCallbackFlags status_flags,
                     void* userdata);

int mp3_callback(const void* input, void* output, u_long frames, const PaStreamCallbackTimeInfo* time_info,
        PaStreamCallbackFlags status_flags, void* userdata) {
    static std::vector<short> buffer;
    static std::vector<float> fft_buffer;

    mp3_stream* stream = static_cast<mp3_stream*>(userdata);
    const auto& mp3 = stream->get_header();
    short* out = static_cast<short*>(output);

    const size_t req_frames = mp3.channels*frames;

    static std::vector<short> delay(1024,0);  // This is a delay buffer so the analysis matches the playback
    if(delay.size() != req_frames) delay.resize(req_frames);

    if(buffer.size() != req_frames) buffer.resize(req_frames);
    if(fft_buffer.size() != req_frames) fft_buffer.resize(req_frames);

    buffer = delay;

    auto len = stream->read(delay, req_frames);
    if(len == 0) { LOG("Complete"); return paComplete; }

    len = buffer.size();
    //auto len = stream->read(buffer, req_frames);
    //if(len == 0) { LOG("Complete"); return paComplete; }

    std::transform(delay.begin(), delay.begin()+len, fft_buffer.begin(), [](short sample) { return sample*inv_s16; });

    // Summarise the stream for display
    {
        unique_lock lock(track_mux);
        if(next_track) { next_track = false; return paComplete; }

        fft_samples = fft_buffer;
        new_samples = true;

        if(sample_count < track_sample_buffer.size()) {
            float sample = 0;
            for(auto i = 0; i < req_frames; i += mp3.channels) {
                sample += fft_buffer[i];
            }
            track_sample_buffer[sample_count] = sample/(frames/mp3.channels);
            ++sample_count;
        }
    }

    if(terminate) return paComplete;

    /*
    if(!t.is_running()) t.start();
    if(last < int(t.get_time<float>())) {
        last = int(t.get_time<float>());
        LOG(t.get_time<double>());
    }
    */

    std::copy(buffer.begin(), buffer.begin()+len, out);
    return paContinue;
}

fft fourier(512);
//constexpr static float box_smooth[5] = { 1.f, 1.f, 1.f, 1.f, 1.f };
constexpr static float tri_smooth[5] = { 1.f, 2.f, 3.f, 2.f, 1.f };
std::vector<float> smoothing(512*5,0);       // 5 point smoothing (tri or box)

using namespace zap;
using namespace zap::maths;
using namespace zap::engine;

using pos3_t = core::position<vec3f>;
using nor3_t = core::normal<vec3f>;
using tex2_t = core::texcoord1<vec2f>;
using col3_t = core::colour1<vec3f>;

using vertex_t = vertex<pos3_t, nor3_t, tex2_t, col3_t>;
using vertex_buffer_t = vertex_buffer<vertex_t, buffer_usage::BU_DYNAMIC_DRAW>;
using vtx_stream_t = vertex_stream<vertex_buffer_t>;
using mesh_t = mesh<vtx_stream_t, primitive_type::PT_TRIANGLES>;

using ublock1 = uniform_block<
        core::cam_position<vec3f>,
        core::cam_view<vec3f>,
        core::mv_matrix<mat4f>,
        core::cam_projection<mat4f>>;
using transform_buffer = uniform_buffer<ublock1, buffer_usage::BU_DYNAMIC_DRAW>;

std::unique_ptr<program> prog2;
vertex_buffer_t vbuffer_blob;
mesh_t mesh_blob;
transform_buffer transform_blob;
texture tex_blob;
std::vector<vertex_t> surf_blob;

mat4f mv_matrix = {
        6.4f, 0.f,  0.f, 0.f,
        0.f, 6.4f,  0.f, 0.f,
        0.f,  0.f, 6.4f, 0.f,
        0.f,  0.f,  0.f, 1.f
};

// Gather some basic summarisations of the FFT

struct analysis_t {
    float beat_min;
    float beat_max;
    float beat_curr;
} analysis;

void run_fft(size_t sample_rate, size_t channels, const std::vector<float>& window, std::vector<float>& bins) {
    constexpr static float inv_s =1.f/512.f;
    constexpr static float inv_tri = 1.f/9.f; // or 1/5 for box smoothing;
    constexpr static float inv_scale = 1.f/80.f;

    if(bins.size() != window.size()) bins.resize(window.size());
    if(fourier.process_samples(window.data(), channels, bins)) {
        for(int i = 0; i != 512; ++i) {
            const auto idx = 2*i;
            float mag = 20.f * std::log10f(2.f * inv_s * std::sqrt(bins[idx]*bins[idx] + bins[idx+1]*bins[idx+1]));
            for(int k = 4; k != 0; --k) smoothing[5*i+k] = smoothing[5*i+(k-1)];
            smoothing[5*i] = mag; mag = 0;
            for(int k = 0; k != 5; ++k) mag += tri_smooth[k]*smoothing[5*i+k];
            mag *= inv_tri;
            bins[i] = (mag + 80.f)*inv_scale;
        }
    }

    // Take the first four bins and use to approximate beat
    analysis.beat_curr = 0;
    for(int i = 0; i != 5; ++i) analysis.beat_curr += .2f*bins[i];
}

void play_mp3(const std::string& path) {
    LOG("GETTING FILES");

    std::vector<std::string> files;
    get_files(path, files);

    LOG("Playing", files.size(), "files");
    for(const auto& f : files) {
        LOG("Playing - ", f);

        mp3_stream mstream(f, 4 * 1024, nullptr);
        if(!mstream.start() || !mstream.is_open()) {
            LOG_ERR("Error opening MP3 File:", f);
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

        while(Pa_IsStreamActive(stream) > 0 && !terminate) Pa_Sleep(200);

        if(Pa_Terminate() != paNoError) {
            LOG_ERR("Pa_Terminate error:", err, Pa_GetErrorText(err));
            return;
        }

        if(terminate) break;
    }
}

static void on_error(int error, const char* description) {
    LOG_ERR("GLFW Error:", error, "Description:", description);
}

#define GLSL(src) "#version 330 core\n" #src

const char* const vtx_src = GLSL(
        uniform mat4 proj_matrix;
        in vec2 position;
        out vec2 st;

        void main() {
            st = vec2(position.x/20.0 + 0.5, position.y/2.0 + 0.5);
            gl_Position = proj_matrix*vec4(position.x, position.y, 0.0, 1.0);
        }
);

const char* const frg_src = GLSL(
        out vec4 frag_colour;
        in vec2 st;

        void main() {
            frag_colour = vec4(st.x, st.y, 1.0 - st.x, 1.0);
        }
);

using namespace zap::maths;
using namespace zap::engine;

const char* mp3file = "/Users/otgaard/Development/zap/output/assets/chembros4.mp3";

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

    std::vector<shader_ptr> arr2;
    arr2.push_back(std::make_shared<shader>(shader_type::ST_VERTEX, vtx_ub_src));
    arr2.push_back(std::make_shared<shader>(shader_type::ST_FRAGMENT, frg_ub_src));
    prog2 = std::make_unique<program>(std::move(arr2));
    prog2->link(true);
    gl_error_check();

    prog2->bind();
    auto loc = prog2->uniform_location("tex");
    glUniform1i(loc, 0);

    tex_blob.allocate();
    auto checker = zap::generators::planar<rgb888_t>::make_checker<vec3b>(32, 32, vec3b(255, 255, 0), vec3b(255, 0, 255));
    tex_blob.initialise(32, 32, checker, true);
    tex_blob.bind();

    mesh_blob.set_stream(vertex_stream<vertex_buffer_t>(&vbuffer_blob));
    mesh_blob.allocate();
    mesh_blob.bind();
    vbuffer_blob.allocate();
    vbuffer_blob.bind();
    vbuffer_blob.initialise(15000);
    //vbuffer.initialise(box);

    auto fnc = [](float x, float y, float z) {
        vec3f v(x-0.5f, y-0.5f, z-0.5f);
        vec3f u(x-0.7f, y-0.5f, z-0.5f);
        return 0.5f/dot(v,v) + 0.2f/dot(u,u);
    };
    surf_blob.reserve(15000);
    generators::surface<vertex_buffer_t>::marching_cubes(fnc, surf_blob);
    LOG("Finished building surface", surf_blob.size());
    vbuffer_blob.initialise(surf_blob);

    transform_blob.allocate();
    transform_blob.bind();
    transform_blob.initialise(nullptr);
    if(transform_blob.map(buffer_access::BA_WRITE_ONLY)) {
        auto& ref = transform_blob.ref();
        ref.cam_position = vec3f(0, 0, -10);
        ref.cam_view = vec3f(1, 1, 0);
        ref.cam_projection = make_perspective<float>(45.f, 1280.f / 768.f, 1.f, 100.f);
        ref.mv_matrix = make_scale<float>(30,30,10) * make_translation<float>(-.65f, -.65f, 1);
        transform_blob.unmap();
    }

    loc = prog2->uniform_block_index("transform");
    transform_blob.bind_point(loc);
    transform_blob.release();
    mesh_blob.release();

    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    zap::maths::mat4<float> proj_matrix = {
            100/1280.f,       0.f, 0.f, 0.f,
            0.f,        200/768.f, 0.f, 0.f,
            0.f,              0.f, 2.f, 0.f,
            0.f,              0.f, 0.f, 1.f
    };

    prog->bind();
    loc = prog->uniform_location("proj_matrix");
    glUniformMatrix4fv(loc, 1, GL_FALSE, proj_matrix.data());
    prog->release();
    gl_error_check();

    using p3_t = zap::core::position<vec3f>;
    using pos3_t = vertex<p3_t>;
    using p3_static_mesh = mesh<vertex_stream<vertex_buffer<pos3_t, buffer_usage::BU_STATIC_DRAW>>, primitive_type::PT_LINE_LOOP>;
    using p3_dynamic_mesh = mesh<vertex_stream<vertex_buffer<pos3_t, buffer_usage::BU_DYNAMIC_DRAW>>, primitive_type::PT_LINE_STRIP>;

    using n3_t = zap::core::normal<vec3f>;
    using t2_t = zap::core::texcoord1<vec2f>;
    using c3_t = zap::core::colour1<vec3f>;

    using vertex_t = vertex<p3_t, n3_t, t2_t, c3_t>;
    using vbuffer_t = vertex_buffer<vertex_t, buffer_usage::BU_DYNAMIC_DRAW>;
    using mesh_t = mesh<vertex_stream<vbuffer_t>, primitive_type::PT_TRIANGLES>;

    vertex_buffer<pos3_t, buffer_usage::BU_STATIC_DRAW> frame;
    vertex_buffer<pos3_t, buffer_usage::BU_DYNAMIC_DRAW> graph;
    vertex_buffer<pos3_t, buffer_usage::BU_DYNAMIC_DRAW> fft_graph;

    std::vector<pos3_t> box;
    box.push_back(pos3_t({{-10,-1,0}}));
    box.push_back(pos3_t({{10,-1,0}}));
    box.push_back(pos3_t({{10,1,0}}));
    box.push_back(pos3_t({{-10,1,0}}));

    mesh_t foo;

    p3_static_mesh frame_mesh(&frame);
    frame_mesh.allocate();
    frame_mesh.bind();
    frame.allocate();
    frame.bind();
    frame.initialise(box);
    frame_mesh.release();

    p3_dynamic_mesh graph_mesh(&graph);
    graph_mesh.allocate();
    graph_mesh.bind();
    graph.allocate();
    graph.bind();
    graph.initialise(2000);
    graph.map(buffer_access::BA_WRITE_ONLY);
    for(int i = 0; i < 2000; ++i) {
        (graph.begin()+i)->position.x = -10.f + i*1/100.f;
        (graph.begin()+i)->position.y = 0.f;
        (graph.begin()+i)->position.z = 0.f;
    }
    graph.unmap();
    graph_mesh.release();

    p3_dynamic_mesh fft_mesh(&fft_graph);
    fft_mesh.allocate();
    fft_mesh.bind();
    fft_graph.allocate();
    fft_graph.bind();
    fft_graph.initialise(64);
    fft_graph.map(buffer_access::BA_WRITE_ONLY);
    for(int i = 0; i != 64; ++i) {
        auto& ref = *(fft_graph.begin()+i);
        ref.position = vec3f(-10.f + i * 20/63.f, 0.f, 0.f);
    }
    fft_graph.unmap();
    fft_mesh.release();

    std::unique_ptr<std::thread> thread_ptr;
    LOG("Starting thread");
    if(argc == 2) {
        thread_ptr = std::make_unique<std::thread>(play_mp3, std::string(argv[1]));
    }

    std::vector<float> track_copy(sample_buffer_size, 0);
    std::vector<float> fft_sample_copy(1024);
    std::vector<float> fft_bins(1024);
    bool process_fft = false;
    analysis_t analysis_copy;


    static float angle = 0.f;
    static float dx = 0.0f;
    static float dy = 0.0f;

    static float offset = 0.0f;
    static int dir = 1;

    static timer t(true);
    float prev = t.getf();
    float curr = t.getf();
    float dt = curr - prev;

    size_t tsamples = 0;
    while(!glfwWindowShouldClose(window)) {
        prev = curr;
        curr = t.getf();
        dt = curr - prev;
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        prog->bind();
        frame_mesh.bind();
        frame_mesh.draw();
        frame_mesh.release();

        {
            unique_lock lock(track_mux);
            if(glfwGetKey(window, GLFW_KEY_N)) next_track = true;

            if(sample_count != 0) {
                std::copy(track_sample_buffer.begin(), track_sample_buffer.begin() + sample_count, track_copy.begin());
                tsamples = sample_count;
                sample_count = 0;
            }

            if(new_samples) {
                fft_sample_copy = fft_samples;
                new_samples = false;
                process_fft = true;
            }
        }

        if(process_fft) {
            analysis_copy = analysis;
            run_fft(44100, 2, fft_sample_copy, fft_bins);
            fft_graph.bind();
            fft_graph.map(buffer_access::BA_READ_WRITE);
            for(int i = 0; i != 64; ++i) (fft_graph.begin() + i)->position.y = fft_bins[i];
            fft_graph.unmap();
            fft_graph.release();
            process_fft = false;
        }

        fft_mesh.bind();
        fft_mesh.draw();
        fft_mesh.release();

        graph.bind();
        graph.map(buffer_access::BA_READ_WRITE);
        if(tsamples != 0) {
            for(auto it = graph.end() - tsamples - 1, end = graph.begin() - tsamples - 1; it != end; --it) {
                it->position.y = (it - tsamples)->position.y;
            }

            for(auto it = track_copy.begin(), end = track_copy.begin() + tsamples; it != end; ++it) {
                (graph.begin() + (it - track_copy.begin()))->position.y = (*it);
            }
            tsamples = 0;
        }
        graph.unmap();
        graph.release();

        /*
        if(analysis_copy.beat_curr > 0.8f) {
            graph_mesh.bind();
            graph_mesh.draw();
            graph_mesh.release();
        }
        */
        prog->release();

        // Move the metaball.
        transform_blob.bind();
        if(transform_blob.map(buffer_access::BA_WRITE_ONLY)) {
            transform_blob.ref().mv_matrix = make_scale<float>(20,20,10) *
                    make_rotation<float>(vec3f(0,0,1), fft_bins[2]) * make_translation<float>(-.5f, -.5f, fft_bins[1]);
            transform_blob.ref().cam_position.z = analysis_copy.beat_curr > 0.8f ? lerp(5.f,10.f,(analysis_copy.beat_curr-0.8f)/.4f) : 1.f;
            transform_blob.unmap();
        }
        transform_blob.release();

        auto fnc = [fft_bins](float x, float y, float z) {
            vec3f v(x-0.8f*fft_bins[0], y-0.8f*fft_bins[1], z-0.5f-0.2f*fft_bins[2]);
            vec3f u(x-0.8f*fft_bins[2], y-0.8f*fft_bins[3], z-0.5f-0.2f*fft_bins[4]);
            vec3f w(x-0.8f*fft_bins[4], y-0.8f*fft_bins[5], z-0.5f-0.2f*fft_bins[6]);
            vec3f r(x-0.8f*fft_bins[6], y-0.8f*fft_bins[7], z-0.5f-0.2f*fft_bins[8]);

            return fft_bins[9]*0.3f/dot(v,v) + 0.25f*fft_bins[10]/dot(u,u) + 0.2f*fft_bins[11]/dot(w,w) + 0.1f*fft_bins[12]/dot(r,r);
        };
        surf_blob.clear();
        generators::surface<vertex_buffer_t>::marching_cubes(fnc, surf_blob);

        dx = 0.5f + 0.2f*std::cos(2.f*angle) + 0.01f*offset;
        dy = 0.5f + 0.2f*std::sin(2.f*angle);

        vbuffer_blob.bind();
        if(vbuffer_blob.map(buffer_access::BA_WRITE_ONLY)) {
            std::copy(surf_blob.begin(), surf_blob.end(), vbuffer_blob.begin());
            vbuffer_blob.unmap();
        }
        vbuffer_blob.release();

        angle += dt;
        angle = angle > 2*maths::PI ? 0 : angle;

        offset += dir*11*dt;
        if(offset < 2.f*PI) dir = 1;
        if(offset > 2.f*PI) dir = -1;

        // Draw the metaballs
        prog2->bind();
        mesh_blob.bind();
        mesh_blob.draw(primitive_type::PT_TRIANGLES, 0, surf_blob.size());
        mesh_blob.release();
        prog2->release();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    terminate = true;

    if(thread_ptr) thread_ptr->join();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
