/* Created by Darren Otgaar on 2016/10/29. http://www.github.com/otgaard/zap */
#include "sampler.hpp"
#include "gl_api.hpp"

using namespace zap::engine;

bool sampler::allocate() {
    gl::glGenSamplers(1, &id_);
    LOG("Sampler Allocated:", id_);
    return is_allocated() && !gl_error_check();
}

void sampler::deallocate() {
    gl::glDeleteSamplers(1, &id_);
    LOG("Sampler Deallocated:", id_);
    id_ = INVALID_RESOURCE;
}

void sampler::initialise() {
    set_wrap_s(tex_wrap::TW_REPEAT);
    set_wrap_t(tex_wrap::TW_REPEAT);
    set_wrap_r(tex_wrap::TW_REPEAT);
    set_mag_filter(tex_filter::TF_NEAREST);
    set_min_filter(tex_filter::TF_NEAREST);
    set_anisotropy(1.f);
    set_border_colour(0.f, 0.f, 0.f, 0.f);
}

void zap::engine::sampler::bind(uint32_t unit) {
    gl::glBindSampler(unit, id_);
}

void zap::engine::sampler::release(uint32_t unit) {
    gl::glBindSampler(unit, 0);
}

void zap::engine::sampler::set_wrap_s(tex_wrap w) {
    gl::glSamplerParameteri(id_, gl::gl_type(tex_parm::TP_WRAP_S), gl::gl_type(w));
}

void zap::engine::sampler::set_wrap_t(tex_wrap w) {
    gl::glSamplerParameteri(id_, gl::gl_type(tex_parm::TP_WRAP_T), gl::gl_type(w));
}

void zap::engine::sampler::set_wrap_r(tex_wrap w) {
    gl::glSamplerParameteri(id_, gl::gl_type(tex_parm::TP_WRAP_R), gl::gl_type(w));
}

void zap::engine::sampler::set_mag_filter(tex_filter f) {
    assert(f == tex_filter::TF_LINEAR || f == tex_filter::TF_NEAREST && "Mag filter must be NEAREST or LINEAR");
    gl::glSamplerParameteri(id_, gl::gl_type(tex_parm::TP_MAG_FILTER), gl::gl_type(f));
}

void zap::engine::sampler::set_min_filter(tex_filter f) {
    gl::glSamplerParameteri(id_, gl::gl_type(tex_parm::TP_MIN_FILTER), gl::gl_type(f));
}

void zap::engine::sampler::set_anisotropy(float v) {
    gl::glSamplerParameterf(id_, gl::gl_type(tex_parm::TP_MAX_ANISOTROPY), v);
}

void sampler::set_border_colour(float r, float g, float b, float a) {
    float colour[4] = {r,g,b,a};
    gl::glSamplerParameterfv(id_, gl::gl_type(tex_parm::TP_BORDER_COLOUR), colour);
}
