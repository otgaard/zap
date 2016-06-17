//
// Created by Darren Otgaar on 2016/05/15.
//

#ifndef ZAP_SHADER_SRC_HPP
#define ZAP_SHADER_SRC_HPP

#define GLSL(src) "#version 330 core\n" #src

const char* vtx_src = GLSL(
        uniform mat4 proj_matrix;
        uniform mat4 mv_matrix;
        in vec2 position;

        out vec2 pos;

        void main() {
            pos = position;
            gl_Position = proj_matrix * mv_matrix * vec4(position.x, position.y, 0.0, 1.0);
        }
);

const char* frg_src = GLSL(
        uniform vec3 colour;
        out vec4 frag_colour;

        in vec2 pos;

        void main() {
            frag_colour = vec4(mix(vec3(0,0,0), colour, max(1 - 2.0f*length(vec2(0.5) - pos), 0)), 1);
        }
);

#endif //ZAP_SHADER_SRC_HPP
