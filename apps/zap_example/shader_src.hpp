//
// Created by Darren Otgaar on 2016/05/15.
//

#ifndef ZAP_SHADER_SRC_HPP
#define ZAP_SHADER_SRC_HPP

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

const char* vtx_ub_src = GLSL(
    in vec2 position;
    in vec2 texcoord1;

    layout (std140) uniform ublock1 {
        vec3 cam_position;
        vec3 cam_view;
        mat4 mv_matrix;
        mat4 proj_matrix;
    };

    out vec2 texcoord;
    out vec3 colour;

    void main() {
        texcoord = texcoord1;
        colour = cam_view;
        gl_Position = proj_matrix * mv_matrix * vec4(position.x, position.y, 0, 1.0);
    }
);

const char* frg_ub_src = GLSL(
    uniform sampler2D tex;

    in vec3 colour;
    in vec2 texcoord;

    out vec4 fragColour;
    void main() {
        fragColour = texture(tex, texcoord) + 0.1*vec4(colour, 1.0);
    }
);

#endif //ZAP_SHADER_SRC_HPP
