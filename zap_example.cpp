/* Created by Darren Otgaar on 2016/03/05. http://www.github.com/otgaard/zap */
#include <iostream>
#include <GLFW/glfw3.h>

#include "maths/vec.hpp"
#include "maths/vec3.hpp"

#include <tools/log.hpp>

using namespace zap::maths;

static void on_error(int error, const char* description) {
    std::cerr << "Error code: " << error << "desc:" << description << std::endl;
}

int main(int argc, char* argv[]) {
    glfwSetErrorCallback(::on_error);

    LOG("Does this work?", 123);

    if(!glfwInit()) return -1;

    glfwWindowHint(GLFW_SAMPLES, 8);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    auto window = glfwCreateWindow(1280, 768, "zap_example", nullptr, nullptr);
    if(!window) {
        std::cerr << "Error creating window - terminating" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
    glfwSwapInterval(0);

    while(!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
