/* Created by Darren Otgaar on 2016/03/05. http://www.github.com/otgaard/zap */

#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#endif //__APPLE__

#include <GLFW/glfw3.h>
#define LOGGING_ENABLED
#include <tools/log.hpp>
#include <engine/shader.hpp>
#include <vector>
#include <engine/program.hpp>
#include <maths/mat4.hpp>
#include <maths/vec4.hpp>
#include <maths/vec3.hpp>
#include <maths/vec2.hpp>
#include <engine/buffer_format.hpp>
#include <maths/functions.hpp>

#include "engine/vertex_buffer.hpp"

static void on_error(int error, const char* description) {
    LOG_ERR("GLFW Error:", error, "Description:", description);
}

#define GLSL(src) "#version 150 core\n" #src

const char* vtx_src = GLSL(
    uniform mat4 proj_matrix;
    in vec4 position;

    void main() {
        gl_Position = proj_matrix*vec4(position.x, position.y, position.z, 1.0);
    }
);

const char* frg_src = GLSL(
    out vec4 frag_colour;

    void main() {
        frag_colour = vec4(1.0, 1.0, 1.0, 1.0);
    }
);

const char* vtx2_src = GLSL(
    uniform mat4 proj_matrix;
    in vec3 position;
    void main() {
        gl_Position = proj_matrix*(vec4(position.x, position.y, position.z, 1.0));
    }
);

const char* frg2_src = GLSL(
    out vec4 frag_colour;
    void main() {
        frag_colour = vec4(1,1,1,1);
    }
);

int main(int argc, char* argv[]) {
    glfwSetErrorCallback(::on_error);

    if(!glfwInit()) return -1;

    glfwWindowHint(GLFW_SAMPLES, 8);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    auto window = glfwCreateWindow(1280, 768, "zap_example", nullptr, nullptr);
    if(!window) {
        LOG_ERR("Error creating window - terminating");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
    glfwSwapInterval(0);

    glViewport(0, 0, 1280, 768);

    using namespace zap::engine;

    std::vector<shader_ptr> arr;
    arr.push_back(std::make_shared<shader>(shader_type::ST_VERTEX, vtx_src));
    arr.push_back(std::make_shared<shader>(shader_type::ST_FRAGMENT, frg_src));

    auto prog = std::make_shared<program>(std::move(arr));
    prog->link(true);

    arr = std::vector<shader_ptr>();
    arr.push_back(std::make_shared<shader>(shader_type::ST_VERTEX, vtx2_src));
    arr.push_back(std::make_shared<shader>(shader_type::ST_FRAGMENT, frg2_src));

    auto prog2 = std::make_shared<program>(std::move(arr));
    prog2->link(true);

    zap::maths::mat4<float> proj_matrix = {
        80/1280.f,      0.f, 0.f, 0.f,
        0.f,       80/768.f, 0.f, 0.f,
        0.f,            0.f, 2.f, 0.f,
        0.f,            0.f, 0.f, 1.f
    };

    prog->bind();
    auto loc = prog->uniform_location("proj_matrix");
    glUniformMatrix4fv(loc, 1, GL_FALSE, proj_matrix.data());
    prog->release();
    gl_error_check();

    prog2->bind();
    loc = prog2->uniform_location("proj_matrix");
    glUniformMatrix4fv(loc, 1, GL_FALSE, proj_matrix.data());
    prog2->release();
    gl_error_check();

    using namespace zap::maths;

    // Now let's test it in OpenGL
    using position_3t = vertex_attribute<vec3f, attribute_type::AT_POSITION>;
    using pos3_t = vertex<position_3t>;
    using vertex_t = vertex_buffer<pos3_t, buffer_usage::BU_STREAM_DRAW>;
    vertex_t buffer1;
    vertex_t buffer2;
    std::vector<pos3_t> disc(1000);

    constexpr float start_x = -10.f;
    const size_t vertex_count = disc.size();
    const float inv_count = 1.f/vertex_count;
    const float delta_x = 20.0f*inv_count;
    float offset = start_x;
    for(auto& v : disc) {
        v.position.x = offset;
        v.position.y = 0.f;
        v.position.z = 0.f;
        offset += delta_x;
    }

    GLuint vao1;
    glGenVertexArrays(1, &vao1);
    glBindVertexArray(vao1);
    gl_error_check();

    buffer1.allocate();
    buffer1.bind();
    buffer1.initialise(disc);

    glBindVertexArray(0);
    gl_error_check();

    GLuint vao2;
    glGenVertexArrays(1, &vao2);
    glBindVertexArray(vao2);
    gl_error_check();

    buffer2.allocate();
    buffer2.bind();
    buffer2.initialise(disc);

    glBindVertexArray(0);
    gl_error_check();

    oscillator<float> wave(10);
    size_t active = 0;

    while(!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        if(active == 0) {
            buffer1.copy(buffer2, 0, 1, buffer2.vertex_count()-1);

            buffer1.bind();
            buffer1.map(buffer_access::BA_WRITE_ONLY);
            buffer1.begin()->position.y = 10.f * (wave.get_value() - .5f);
            offset = start_x;
            for(auto& v : buffer1) {
                v.position.x = offset;
                offset += delta_x;
            }

            buffer1.unmap();
            buffer1.release();

            prog2->bind();
            glBindVertexArray(vao2);
            glDrawArrays(GL_LINE_STRIP, 0, buffer2.vertex_count());
            gl_error_check();
            glBindVertexArray(0);
            prog2->release();
            active = 1;
        } else {
            buffer2.copy(buffer1, 0, 1, buffer1.vertex_count()-1);

            buffer2.bind();
            buffer2.map(buffer_access::BA_WRITE_ONLY);
            buffer2.begin()->position.y = 10.f * (wave.get_value() - .5f);
            offset = start_x;
            for(auto& v : buffer2) {
                v.position.x = offset;
                offset += delta_x;
            }
            buffer2.unmap();
            buffer2.release();

            prog2->bind();
            glBindVertexArray(vao1);
            glDrawArrays(GL_LINE_STRIP, 0, buffer1.vertex_count());
            gl_error_check();
            glBindVertexArray(0);
            prog2->release();
            active = 0;
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
