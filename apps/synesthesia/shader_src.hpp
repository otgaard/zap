//
// Created by Darren Otgaar on 2016/06/08.
//

#ifndef ZAP_SHADER_SRC_HPP
#define ZAP_SHADER_SRC_HPP

#define GLSL(src) "#version 330 core\n" #src

const char* vtx_ub_src = GLSL(
    in vec3 position;
    in vec3 normal;
    in vec2 texcoord1;

    layout (std140) uniform transform {
        vec3 cam_position;
        vec3 cam_view;
        mat4 mv_matrix;
        mat4 proj_matrix;
    };

    out vec3 nor;
    out vec2 texcoord;
    out vec3 colour;
    out float intensity;

    void main() {
        intensity = cam_position.z;
        nor = normal;
        texcoord = texcoord1;
        colour = cam_view;
        gl_Position = proj_matrix * mv_matrix * vec4(position.x, position.y, position.z, 1.0);
    }
);

const char* frg_ub_src = GLSL(
    uniform sampler2D tex;

    in float intensity;
    in vec3 nor;
    in vec3 colour;
    in vec2 texcoord;

    out vec4 fragColour;
    void main() {
        float s = max(dot(nor, normalize(vec3(0,10,10))), 0);
        fragColour = s*(0.8*intensity*texture(tex, texcoord) + 0.2*vec4(colour, 1.0));
    }
);

#endif //ZAP_SHADER_SRC_HPP
