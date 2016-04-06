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
    out vec2 tex1;
    void main() {
        tex1 = texcoord1;
        gl_Position = proj_matrix*(vec4(position.x, position.y, position.z, 1.0));
    }
);

const char* frg2_src = GLSL(
    in vec2 tex1;
    out vec4 frag_colour;
    void main() {
        frag_colour = mix(vec4(1,0,0,1), vec4(1,1,0,1), tex1.y);
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

    float tri[3][3] = {
        { -1.f, -0.5f, 0.5f },
        { -1.f, +0.5f, 0.5f },
        { +1.f,   0.f, 0.5f }
    };

    GLuint vao;
    glGenVertexArrays(1, &vao);
    if(vao == INVALID_RESOURCE) LOG_ERR("Could not allocate vertex array");
    glBindVertexArray(vao);

    gl_error_check();

    GLuint buf;
    glGenBuffers(1, &buf);
    if(buf == INVALID_RESOURCE) LOG_ERR("Could not allocate vertex buffer");
    glBindBuffer(GL_ARRAY_BUFFER, buf);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tri), (void*)tri, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glBindVertexArray(0);

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

    /*
    using pos3_t = vertex_attribute<vec3f, attribute_type::AT_POSITION>;
    using nor3_t = vertex_attribute<vec2f, attribute_type::AT_NORMAL>;
    using tan3_t = vertex_attribute<vec3f, attribute_type::AT_TANGENT>;
    using bin3_t = vertex_attribute<vec3f, attribute_type::AT_BINORMAL>;
    using tst4_t = vertex_attribute<vec4f, attribute_type::AT_TEXCOORD1>;
    using type_t = vertex<pos3_t, nor3_t, tan3_t, bin3_t, tst4_t>;

    type_t vertex{{{1.0f,2.0f,3.0f}}, {{10.0f,30.0f}}, {{100.0f,200.0f,300.0f}}, {{0.f,0.f,0.f}}, {{1.f,2.f,3.f,4.f}}};
    LOG(vertex.position.x, vertex.position.y, vertex.position.z);
    LOG(vertex.normal.x, vertex.normal.y);
    LOG(vertex.texcoord1.x, vertex.texcoord1.y, vertex.texcoord1.z, vertex.texcoord1.w);

    vertex.texcoord1 = vec4f(10.f,20.f,30.f,40.f);
    LOG(vertex.texcoord1.x, vertex.texcoord1.y, vertex.texcoord1.z, vertex.texcoord1.w);

    get<0>(vertex).x = 123.321f;
    LOG(std::remove_reference<decltype(get_attrib<2>(vertex))>::pod_t::AT_CODE);
    LOG(type_t::typecode<2>());
    LOG(type_t::size());
    LOG(type_t::attrib_size<0>(), type_t::attrib_size<1>());
    LOG(vertex.position.x, vertex.tangent.y);
    LOG("offset =", type_t::attrib_offset<4>());

    LOG("sizeof=", sizeof(type_t), sizeof(vertex), sizeof(vec3f)*2+sizeof(vec2f));
    */

    // Now let's test it in OpenGL
    using position_3t = vertex_attribute<vec3f, attribute_type::AT_POSITION>;
    using texcoord1_2t = vertex_attribute<vec2f, attribute_type::AT_TEXCOORD1>;
    using pos3tex2_t = vertex<position_3t, texcoord1_2t>;

    pos3tex2_t triangle[3] = {
            {
                    {{-10.f,-10.f,0.f}}, {{0.f,0.f}}
            },
            {
                    {{-10.f,10.f,0.f}}, {{0.f,1.f}}
            },
            {
                    {{10.f,0.f,0.f}}, {{1.f,0.5f}}
            }
    };

    LOG(sizeof(triangle), sizeof(vec3f)*3 + sizeof(vec2f)*3);

    GLuint vao2;
    glGenVertexArrays(1, &vao2);
    if(vao2 == INVALID_RESOURCE) LOG_ERR("Could not allocate vertex array");
    glBindVertexArray(vao2);

    gl_error_check();

    GLuint buf2;
    glGenBuffers(1, &buf2);
    if(buf2 == INVALID_RESOURCE) LOG_ERR("Could not allocate vertex buffer");
    glBindBuffer(GL_ARRAY_BUFFER, buf2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), (void*)triangle, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(pos3tex2_t), (void*)pos3tex2_t::attrib_offset<0>());
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(pos3tex2_t), (void*)pos3tex2_t::attrib_offset<1>());
    gl_error_check();
    LOG("offsets =", pos3tex2_t::attrib_offset<0>(), pos3tex2_t::attrib_offset<1>());
    glBindVertexArray(0);

    vertex_buffer<pos3tex2_t, buffer_usage::BU_STATIC_DRAW> my_buffer;




    while(!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        glBindVertexArray(vao2);
        prog2->bind();
        glDrawArrays(GL_TRIANGLES, 0, 3);
        prog2->release();
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
