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
    in vec2 texcoord1;
    in vec3 colour2;
    out vec2 tex1;
    out vec3 col;
    void main() {
        tex1 = texcoord1;
        col = colour2;
        gl_Position = proj_matrix*(vec4(position.x, position.y, position.z, 1.0));
    }
);

const char* frg2_src = GLSL(
    in vec2 tex1;
    in vec3 col;
    out vec4 frag_colour;
    void main() {
        frag_colour = mix(vec4(col,1), vec4(1,1,0,1), tex1.y);
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
    using texcoord1_2t = vertex_attribute<vec2f, attribute_type::AT_TEXCOORD1>;
    using colour2_3t = vertex_attribute<vec3f, attribute_type::AT_COLOUR2>;
    using pos3tex2col3_t = vertex<position_3t, texcoord1_2t, colour2_3t>;
    using vertex_t = vertex_buffer<pos3tex2col3_t, buffer_usage::BU_STATIC_DRAW>;
    vertex_t my_buffer;
    std::vector<pos3tex2col3_t> disc(61);

    float theta = 0.0f;
    const float inc = M_PI/30.f;

    for(auto& v : disc) {
        auto stheta = std::sin(theta); auto ctheta = std::cos(theta);
        v.texcoord1.x = 0.5f*ctheta + 0.5f;
        v.texcoord1.y = 0.5f*stheta + 0.5f;
        v.position.x = 5.f*ctheta + 10.f;
        v.position.y = 5.f*stheta;
        v.position.z = 0;
        v.colour2.x = 1.0f;
        v.colour2.y = 0.5f;
        v.colour2.z = 1.0f;
        theta += inc;
    }

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    gl_error_check();

    my_buffer.allocate();
    my_buffer.bind();
    my_buffer.initialise(disc);

    glBindVertexArray(0);
    gl_error_check();

    while(!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        prog2->bind();
        glBindVertexArray(vao);
        glDrawArrays(GL_LINE_STRIP, 0, my_buffer.vertex_count());
        gl_error_check();
        glBindVertexArray(0);
        prog2->release();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
