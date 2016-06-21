//
// Created by Darren Otgaar on 2016/05/15.
//

#ifndef ZAP_SHADER_SRC_HPP
#define ZAP_SHADER_SRC_HPP

#define GLSL(src) "#version 330 core\n" #src

const char* vtx_src = GLSL(
        uniform mat4 proj_matrix;
        in vec2 position;

        out vec2 pos;

        void main() {
            pos = position;
            gl_Position = proj_matrix * vec4(position.x, position.y, 0.0, 1.0);
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

const char* tex_frg_src = GLSL(
    uniform sampler2D tex;
    out vec4 frag_colour;

    in vec2 pos;

    void main() {
        frag_colour = texture(tex, pos);
    }
);

const char* cube_vtx_src = GLSL(
    layout (std140) uniform transform {
        float scale;
        mat4 mv_matrix;
        mat4 proj_matrix;
    };

    in vec4 position;
    in vec3 normal;
    in vec2 texcoord1;

    out vec3 nor;
    out vec2 tc;

    void main() {
        nor = vec3(mv_matrix*vec4(normal,0));
        tc = texcoord1;
        gl_Position = proj_matrix * mv_matrix * position;
    }
);

const char* cube_frg_src = GLSL(
    uniform sampler2D tex;
    const vec3 light_dir = normalize(vec3(0,1,1));
    in vec3 nor;
    in vec2 tc;
    out vec4 frag_colour;

    void main() {
        float s = max(dot(nor, light_dir), 0);
        frag_colour = vec4(s*texture(tex, tc).xyz, 1);
    }
);

#endif //ZAP_SHADER_SRC_HPP
