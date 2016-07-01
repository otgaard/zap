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

    out vec3 pos;
    out vec3 nor;
    out vec2 tc;

    void main() {
        pos = vec3(position.xy, position.z + scale);
        nor = vec3(mv_matrix*vec4(normal,0));
        tc = texcoord1;
        gl_Position = proj_matrix * mv_matrix * position;
    }
);

const char* cube_frg_src = GLSL(
    const float PI = 3.141593;
    uniform usampler1D tex;
    const vec3 light_dir = normalize(vec3(0,0,1));
    in vec3 pos;
    in vec3 nor;
    in vec2 tc;
    out vec4 frag_colour;

    int prn(int x) { return int(texelFetch(tex, x & 0xFF, 0).r); }
    int prn2(int x, int y) { return prn(x + prn(y)); }
    int prn3(int x, int y, int z) { return prn(x + prn(y + prn(z))); }

    float bilinear1(float u, float v, float P00, float P01, float P10, float P11) {
        float omu = 1.0 - u; return (1.0 - v)*(P00*omu + P01*u) + v*(P10*omu + P11*u);
    }

    float vnoise2(float x, float y) {
        int xi = int(x); int yi = int(y);
        float dx = x - xi; float dy = y - yi;
        return bilinear1(dx, dy, prn2(xi, yi), prn2(xi+1, yi), prn2(xi, yi+1), prn2(xi+1, yi+1))/255.0;
    }

    float vnoise3(float x, float y, float z) {
        int xi = int(floor(x)); int yi = int(floor(y)); int zi = int(floor(z));
        float dx = x - xi; float dy = y - yi; float dz = z - zi;
        return mix(bilinear1(dx, dy, prn3(xi, yi, zi),   prn3(xi+1, yi, zi),   prn3(xi, yi+1, zi),   prn3(xi+1, yi+1, zi)),
                   bilinear1(dx, dy, prn3(xi, yi, zi+1), prn3(xi+1, yi, zi+1), prn3(xi, yi+1, zi+1), prn3(xi+1, yi+1, zi+1)),
                   dz)/255.0;
    }

    float turbulence2(float x, float y, int octaves, float persistence, float lacunarity) {
        float freq = 1.0; float ampl = 1.0; float accum = 0.0; float mag = 0.0;
        for(int i = 0; i != octaves; ++i) {
            accum += abs(ampl * vnoise2(freq * x, freq * y));
            mag += ampl;
            ampl *= persistence;
            freq *= lacunarity;
        }
        return 1.0/mag*accum;
    }

    float turbulence3(float x, float y, float z, int octaves, float persistence, float lacunarity) {
        float freq = 1.0; float ampl = 1.0; float accum = 0.0; float mag = 0.0;
        for(int i = 0; i != octaves; ++i) {
            accum += abs(ampl * (2.0*vnoise3(freq * x, freq * y, freq * z) - 1.0));
            mag += ampl;
            ampl *= persistence;
            freq *= lacunarity;
        }
        return 1.0/mag*accum;
    }

    void main() {
        float s = max(dot(nor, light_dir), 0);
        float v = turbulence3(10*pos.x, 10*pos.y, 10*pos.z, 4, .5, 2.);
        frag_colour = vec4(s*mix(vec3(1,1,0), vec3(1,0,0), v), 1);
        //frag_colour = vec4(s*texture(tex, tc).rgb, 1);
    }
);

#endif //ZAP_SHADER_SRC_HPP
