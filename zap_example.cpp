/* Created by Darren Otgaar on 2016/03/05. http://www.github.com/otgaard/zap */

#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>
#include <tools/log.hpp>
#include <engine/shader.hpp>
#include <vector>
#include <engine/program.hpp>
#include <maths/mat4.hpp>

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

    float tri[3][3] = {
            { -1.f, -0.5f, 0.5f },
            { -1.f, +0.5f, 0.5f },
            { +1.f,   0.f, 0.5f }
    };

    GLuint vao;
    glGenVertexArrays(1, &vao);
    if(vao == INVALID_RESOURCE) LOG_ERR("Could not allocate vertex array");
    glBindVertexArray(vao);

    gl_error_log();

    GLuint buf;
    glGenBuffers(1, &buf);
    if(buf == INVALID_RESOURCE) LOG_ERR("Could not allocate vertex buffer");
    glBindBuffer(GL_ARRAY_BUFFER, buf);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tri), (void*)tri, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glBindVertexArray(0);

    gl_error_log();

    zap::maths::mat4<float> proj_matrix = {
        20/1280.f,      0.f, 0.f, 0.f,
        0.f,       20/768.f, 0.f, 0.f,
        0.f,            0.f, 2.f, 0.f,
        0.f,            0.f, 0.f, 1.f
    };

    prog->bind();
    auto loc = prog->uniform_location("proj_matrix");
    glUniformMatrix4fv(loc, 1, GL_FALSE, proj_matrix.data());
    prog->release();
    gl_error_log();

    while(!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        glBindVertexArray(vao);
        prog->bind();
        glDrawArrays(GL_TRIANGLES, 0, 3);
        prog->release();
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
