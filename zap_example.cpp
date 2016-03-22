/* Created by Darren Otgaar on 2016/03/05. http://www.github.com/otgaard/zap */
#include <GLFW/glfw3.h>
#include <tools/log.hpp>

static void on_error(int error, const char* description) {
    LOG_ERR("GLFW Error:", error, "Description:", description);
}

/*
 * The following builds a simple renderloop I can use for testing in the meantime.
 */

struct render_state {
    GLuint vao;
    GLuint vbo;
    GLuint ibo;
} state;

#define GLSL(src) "#version 150 core\n" #src

const char* vtx_src = GLSL(
    in vec4 position;

    void main() {
        gl_Position = vec4(position.x, position.y, position.z, 1.0);
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

    std::cerr << vtx_src << std::endl;

    while(!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
