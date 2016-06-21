/* Created by Darren Otgaar on 2016/06/10. http://www.github.com/otgaard/zap */
#include "application.hpp"
#define LOGGING_ENABLED
#include <tools/log.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

static void on_error_handler(int error, const char* description) {
    LOG_ERR("GLFW Error:", error, "Description:", description);
}

static void on_resize_handler(GLFWwindow* window_ptr, int width, int height) {
    if(auto app_ptr = reinterpret_cast<application*>(glfwGetWindowUserPointer(window_ptr))) {
        app_ptr->on_resize(width, height);
    }
}

static void on_keypress_handler(GLFWwindow* window_ptr, int key, int scancode, int action, int mods) {
    if(auto app_ptr = reinterpret_cast<application*>(glfwGetWindowUserPointer(window_ptr))) {
        if(action == GLFW_PRESS) app_ptr->on_keypress(char(key));
    }
}

application::application(const std::string& name, int width, int height, bool fullscreen) : sc_width_(width),
    sc_height_(height), fullscreen_(fullscreen), app_name_(name) {
}

int application::run() {
    glfwSetErrorCallback(::on_error_handler);
    if(!glfwInit()) return -1;

    glfwWindowHint(GLFW_SAMPLES, 8);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    auto window = glfwCreateWindow(sc_width_, sc_height_, app_name_.c_str(),
                                   fullscreen_ ? glfwGetPrimaryMonitor() : nullptr, nullptr);
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

    // Set the userdata pointer & callbacks
    glfwSetWindowUserPointer(window, this);
    glfwSetWindowSizeCallback(window, ::on_resize_handler);
    glfwSetKeyCallback(window, ::on_keypress_handler);

    glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
    glfwSwapInterval(1);
    glViewport(0, 0, sc_width_, sc_height_);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    initialise();

    timer_.start();
    double curr_time = timer_.getd();
    double prev_time = curr_time;

    while(!glfwWindowShouldClose(window)) {
        prev_time = curr_time;
        curr_time = timer_.getd();
        float dt = float(curr_time - prev_time);
        update(curr_time, dt);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    shutdown();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

void application::on_keypress(char ch) {
    LOG("KEYPRESS_EVENT", ch);
}

void application::on_resize(int width, int height) {
    sc_width_ = width; sc_height_ = height;
}

void application::depth_test(bool on) {
    on ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
}

void application::bf_culling(bool on) {
    glCullFace(GL_BACK);
    on ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
}

void application::alpha_blending(bool on) {
    if(on) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    } else {
        glDisable(GL_BLEND);
    }
}

void application::clear(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}