/* Created by Darren Otgaar on 2016/03/05. http://www.github.com/otgaard/zap */
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define LOGGING_ENABLED
#include <tools/log.hpp>
#include <maths/mat4.hpp>
#include <maths/vec3.hpp>
#include <maths/vec2.hpp>
#include <engine/types.hpp>
#include <engine/shader.hpp>
#include <engine/program.hpp>
#include <maths/functions.hpp>
#include <engine/vertex_buffer.hpp>

#include <stb_image.h>
#include <engine/pixel_buffer.hpp>

static void on_error(int error, const char* description) {
    LOG_ERR("GLFW Error:", error, "Description:", description);
}

#define GLSL(src) "#version 330 core\n" #src

const char* vtx_src = GLSL(
    uniform mat4 proj_matrix;
    in vec2 position;

    void main() {
        gl_Position = proj_matrix*vec4(position.x, position.y, 0.0, 1.0);
    }
);

const char* frg_src = GLSL(
    uniform vec3 colour;
    out vec4 frag_colour;

    void main() {
        frag_colour = vec4(colour, 1.0);
    }
);

const char* pane_vtx_src = GLSL(
    uniform mat4 proj_matrix;
    in vec3 position;
    in vec3 normal;
    in float point_size;

    out vec2 pos;
    out vec3 nor;
    void main() {
        pos = position.xy * point_size;
        nor = normal;
        gl_Position = proj_matrix*vec4(position, 1.0);
    }
);

const char* pane_frg_src = GLSL(
    out vec4 frag_colour;
    in vec2 pos;
    in vec3 nor;
    void main() {
        frag_colour = vec4(pos.x,pos.y,0,1);
    }
);

const char* tex_vtx_src = GLSL(
    uniform mat4 proj_matrix;
    in vec3 position;
    in vec2 texcoord1;

    out vec2 tex1;
    void main() {
        tex1 = texcoord1;
        gl_Position = proj_matrix*vec4(position, 1.0);
    }
);

const char* tex_frg_src = GLSL(
    uniform sampler2D tex;
    in vec2 tex1;
    out vec4 frag_colour;
    void main() {
        frag_colour = texture(tex, tex1);
    }
);

using namespace zap::engine;

// Higher order function to create a wave function from an input periodic function with range [-1, 1]
template <typename FNC>
struct wave {
    constexpr static auto make_fnc(FNC fnc, float frequency, float amplitude, float phase) {
        return [=](float x) -> float {
            return amplitude * fnc(frequency * x + phase);
        };
    };
};

template <typename IT, typename FNC>
void sample(IT begin, IT end, float start, float stop, FNC fnc) {
    const auto samples = end - begin;
    const auto inc = (stop - start)/samples;
    for(auto it = begin; it != end; ++it) {
        it->position.x = start + inc * (it - begin);
        it->position.y = fnc(it->position.x);
    }
}

#include <engine/index_buffer.hpp>
#include "generator.hpp"

int main(int argc, char* argv[]) {
    glfwSetErrorCallback(::on_error);

    if(!glfwInit()) return -1;

    glfwWindowHint(GLFW_SAMPLES, 8);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    auto window = glfwCreateWindow(1280, 768, "zap_example", nullptr, nullptr);
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

    // PROGRAM 1 SETUP
    std::vector<shader_ptr> arr;
    arr.push_back(std::make_shared<shader>(shader_type::ST_VERTEX, vtx_src));
    arr.push_back(std::make_shared<shader>(shader_type::ST_FRAGMENT, frg_src));

    auto prog = std::make_shared<program>(std::move(arr));
    prog->link(true);
	gl_error_check();

    // PROGRAM 2 SETUP
    std::vector<shader_ptr> arr2;
    arr2.push_back(std::make_shared<shader>(shader_type::ST_VERTEX, pane_vtx_src));
    arr2.push_back(std::make_shared<shader>(shader_type::ST_FRAGMENT, pane_frg_src));

    auto pane_prog = std::make_shared<program>(std::move(arr2));
    pane_prog->link(true);
    gl_error_check();

    // PROGRAM 3 SETUP
    std::vector<shader_ptr> arr3;
    arr3.push_back(std::make_shared<shader>(shader_type::ST_VERTEX, tex_vtx_src));
    arr3.push_back(std::make_shared<shader>(shader_type::ST_FRAGMENT, tex_frg_src));

    auto tex_prog = std::make_shared<program>(std::move(arr3));
    tex_prog->link(true);
    gl_error_check();

    // Texture
    auto tex1 = generator::create_checker();

    zap::maths::mat4<float> proj_matrix = {
        120/1280.f,       0.f, 0.f, 0.f,
        0.f,        120/768.f, 0.f, 0.f,
        0.f,              0.f, 2.f, 0.f,
        0.f,              0.f, 0.f, 1.f
    };

    prog->bind();
    auto loc = prog->uniform_location("proj_matrix");
    glUniformMatrix4fv(loc, 1, GL_FALSE, proj_matrix.data());
    loc = prog->uniform_location("colour");
    auto line_colour = zap::maths::vec3f(0,0,1);
    glUniform3fv(loc, 1, line_colour.data());
    prog->release();
    gl_error_check();

    pane_prog->bind();
    loc = pane_prog->uniform_location("proj_matrix");
    glUniformMatrix4fv(loc, 1, GL_FALSE, proj_matrix.data());
    prog->release();
    gl_error_check();

    // Setup Texture Program & Unit
    tex1->bind();
    tex_prog->bind();
    glActiveTexture(GL_TEXTURE0);
    loc = tex_prog->uniform_location("proj_matrix");
    glUniformMatrix4fv(loc, 1, GL_FALSE, proj_matrix.data());
    gl_error_check();
    loc = tex_prog->uniform_location("tex");
    glUniform1i(loc, 0);
    tex_prog->release();
    tex1->release();
    gl_error_check();

    using namespace zap::maths;
    using namespace zap::engine;

    //static_assert(std::is_trivially_copyable<vec3f>::value, "What?");

    using p3_t = zap::core::position<vec3f>;   // vertex_attribute<vec3f, attribute_type::AT_POSITION>;
    using t2_t = zap::core::texcoord1<vec2f>;  //vertex_attribute<vec2f, attribute_type::AT_TEXCOORD1>;
    using n3_t = zap::core::normal<vec3f>;     //using n3_t = vertex_attribute<vec3f, attribute_type::AT_NORMAL>;
    using ps1_t = zap::core::pointsize<float>; //using ps1_t = vertex_attribute<float, attribute_type::AT_POINTSIZE>;
    using pos3_t = vertex<p3_t>;

    using vertex_t = vertex<p3_t, n3_t, ps1_t>;
    using vertex_buf_t = vertex_buffer<vertex_t, buffer_usage::BU_STATIC_DRAW>;

    std::vector<vertex_t> pane_def = {
            vertex_t(
                    {{-10, -5, 0}}, // pos
                    {{0, 0, 1}},    // normal
                    {-1}            // point-size
            ),
            vertex_t(
                    {{10, -5, 0}},
                    {{0, 0, 1}},
                    {1}
            ),
            vertex_t(
                    {{10, 5, 0}},
                    {{0, 0, 1}},
                    {1}
            ),
            vertex_t(
                    {{-10, 5, 0}},
                    {{0, 0, 1}},
                    {1}
            )
    };

    using p3t2_t = vertex<p3_t, t2_t>;
    using p3t2_buf_t = vertex_buffer<p3t2_t, buffer_usage::BU_STATIC_DRAW>;

    std::vector<p3t2_t> texpane_def = {
            p3t2_t(
                    {{-10, -5, 0}}, // pos
                    {{0, 0}}        // texcoord
            ),
            p3t2_t(
                    {{10, -5, 0}},
                    {{1, 0}}
            ),
            p3t2_t(
                    {{10, 5, 0}},
                    {{1, 1}}
            ),
            p3t2_t(
                    {{-10, 5, 0}},
                    {{0, 1}}
            )
    };

    // Let's build an index of disconnected lines
    std::vector<uint16_t> lines = { 0, 2, 1, 3, 4, 6, 5, 7 };

    GLuint index_test;
    glGenVertexArrays(1, &index_test);
    glBindVertexArray(index_test);

    index_buffer<uint16_t, primitive_type::PT_LINES, buffer_usage::BU_STATIC_DRAW> index1;
    index1.allocate();
    index1.bind();
    index1.initialise(lines);

    index1.map(buffer_access::BA_READ_ONLY);
    for(int i = 0; i != index1.index_count(); ++i) {
        LOG(index1[i]);
    }
    index1.unmap();

    vertex_buffer<pos3_t, buffer_usage::BU_STATIC_DRAW> testbuffer;
    std::vector<pos3_t> testpoints = {
            pos3_t({{-1,-1,0}}), pos3_t({{0,0,0}}), pos3_t({{1,1,0}}), pos3_t({{2,-1,0}}),
            pos3_t({{-3,1,0}}), pos3_t({{-6,-3,0}}), pos3_t({{8,4,0}}), pos3_t({{10,-10,0}})
    };
    testbuffer.allocate();
    testbuffer.bind();
    testbuffer.initialise(testpoints);
    glBindVertexArray(0);
    gl_error_check();

    vertex_buf_t pane;

    vertex_buffer<pos3_t, buffer_usage::BU_STATIC_DRAW> frame;
    vertex_buffer<pos3_t, buffer_usage::BU_DYNAMIC_DRAW> graph;

    std::vector<pos3_t> box;
    box.push_back(pos3_t({{-10,-1,0}}));
    box.push_back(pos3_t({{10,-1,0}}));
    box.push_back(pos3_t({{10,1,0}}));
    box.push_back(pos3_t({{-10,1,0}}));

    LOG(sizeof(pos3_t), pos3_t::offsets::data[0], pos3_t::types::data[0], pos3_t::counts::data[0], pos3_t::datatypes::data[0]);

    GLuint frame_mesh;
    glGenVertexArrays(1, &frame_mesh);
    glBindVertexArray(frame_mesh);
    frame.allocate();
    frame.bind();
    frame.initialise(box);

    LOG("Frame Size = ", frame.vertex_count(), frame.end() - frame.begin());

    frame.bind();
    frame.map(buffer_access::BA_READ_ONLY);

    for(const auto& v : frame) {
        LOG(v.position.x, v.position.y, v.position.z);
    }

    frame.unmap();

    glBindVertexArray(0);
    gl_error_check();

    GLuint line_mesh;
    glGenVertexArrays(1, &line_mesh);
    glBindVertexArray(line_mesh);
    graph.allocate();
    graph.bind();
    graph.initialise(2000);
    glBindVertexArray(0);
    gl_error_check();

    GLuint pane_mesh;
    glGenVertexArrays(1, &pane_mesh);
    glBindVertexArray(pane_mesh);
    pane.allocate();
    pane.bind();
    pane.initialise(pane_def);
    glBindVertexArray(0);
    gl_error_check();

    p3t2_buf_t texpane;
    GLuint tex_mesh;
    glGenVertexArrays(1, &tex_mesh);
    glBindVertexArray(tex_mesh);
    texpane.allocate();
    texpane.bind();
    texpane.initialise(texpane_def);
    glBindVertexArray(0);
    gl_error_check();

    float offset = 0.0f;
    constexpr float pi = zap::maths::PI;
    timer t;
    float prev = t.getf(), curr = t.getf();

    auto sin5Hz = wave<decltype(sinf)>::make_fnc(sinf, 5, .4f, 0.f);
    auto sin17Hz = wave<decltype(sinf)>::make_fnc(sinf, 17, .4f, 0.f);
    auto sin31Hz = wave<decltype(sinf)>::make_fnc(sinf, 31, 0.2f, 0.f);
    auto synth = [&sin5Hz, &sin17Hz, &sin31Hz](float x) {
        return sin5Hz(x) + sin17Hz(x) + sin31Hz(x);
    };

    while(!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        tex_prog->bind();
        glActiveTexture(GL_TEXTURE0);
        tex1->bind();
        glBindVertexArray(tex_mesh);
        glDrawArrays(GL_TRIANGLE_FAN, 0, texpane.vertex_count());
        glBindVertexArray(0);
        tex1->release();
        tex_prog->release();

        prog->bind();
        glBindVertexArray(frame_mesh);
        glDrawArrays(GL_LINE_LOOP, 0, frame.vertex_count());
        glBindVertexArray(0);

        graph.bind();
        graph.map(buffer_access::BA_WRITE_ONLY);
        //LOG(graph[0].position.y, graph[1].position.y, graph[2].position.y);
        sample(graph.begin(), graph.end(), -3*pi, 3*pi, [&offset, synth](float x) {
            x += offset;
            return synth(x);
        });
        graph.unmap();
        graph.release();

        glBindVertexArray(line_mesh);
        glDrawArrays(GL_LINE_STRIP, 0, graph.vertex_count());
        glBindVertexArray(0);

        offset += 5*(curr - prev);
        if(offset >= 2*pi) offset -= 2*pi;

        prev = curr;
        curr = t.getf();

        glBindVertexArray(index_test);
        glDrawElements(GL_LINES, index1.index_count(), GL_UNSIGNED_SHORT, nullptr);
        gl_error_check();
        glBindVertexArray(0);

        prog->release();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
