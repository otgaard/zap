/* Created by Darren Otgaar on 2016/03/05. http://www.github.com/otgaard/zap */
#include <iostream>
#include <GLFW/glfw3.h>

#include "maths/vec.hpp"
#include "maths/vec3.hpp"

using namespace zap::maths;

static void on_error(int error, const char* description) {
    std::cerr << "Error code: " << error << "desc:" << description << std::endl;
}

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
        std::cerr << "Error creating window - terminating" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
    glfwSwapInterval(0);

    constexpr vec3<float> val1 = {1.0f, 2.0f, 3.0f};
    constexpr vec3<float> val2 = {10.f, 20.f, 30.f};
    constexpr vec3<float> r1 = val1 + val2 * val1 - val2;
    static_assert(r1.x == 1 && r1.y == 22 && r1.z == 63, "Something went very, very wrong");
    constexpr vec3<float> r2 = 10.0f*r1;
    static_assert(r2.x == 10 && r2.y == 220 && r2.z == 630, "Something went very, very wrong");

    constexpr vec3<float> foo = (10.0f*vec3<float>({{1.0f, 2.0f, 3.0f}}))/2.0f + 1.0f/vec3<float>({{20.0f, 100.0f, 200.0f}});
    static_assert(foo.x == 5.05f, "This is amazing");

    constexpr float bar = dot(foo, foo);
    std::cerr << "DOT PRODUCT:" << bar << std::endl;
    static_assert(bar < 350.855f, "More static here");

    std::cerr << foo.x << ":" << foo.y << ":" << foo.z << std::endl;

    std::cerr << r1.x << " " << r1.y << " " << r1.z << " " << std::endl;
    std::cerr << r2.x << " " << r2.y << " " << r2.z << " " << std::endl;

    for(const auto& r : val1) {
        std::cerr << "Here:" << r << std::endl;
    }

    while(!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
