/* Created by Darren Otgaar on 2016/05/27. http://www.github.com/otgaard/zap */
#include "framebuffer.hpp"
#include "gl_api.hpp"

using namespace zap::engine;
using namespace zap::engine::gl;

static constexpr std::array<uint32_t, 12> draw_buffers = {{
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT2,
        GL_COLOR_ATTACHMENT3,
        GL_COLOR_ATTACHMENT4,
        GL_COLOR_ATTACHMENT5,
        GL_COLOR_ATTACHMENT6,
        GL_COLOR_ATTACHMENT7,
        GL_COLOR_ATTACHMENT8,
        GL_COLOR_ATTACHMENT9,
        GL_COLOR_ATTACHMENT10,
        GL_COLOR_ATTACHMENT11
}};

framebuffer::framebuffer(framebuffer_type ft) : ftype_(ft) {
}

framebuffer::framebuffer(framebuffer_type ft, attachment_type colour_type, attachment_type depth_type) : ftype_(ft),
                                                                                                         col_attachment_(colour_type),
                                                                                                         ds_attachment_(depth_type) {
}

bool framebuffer::allocate() {
    glGenFramebuffers(1, &framebuffer_);
    LOG("Framebuffer Allocated: ", framebuffer_);
    return is_allocated() && !gl_error_check();
}

void framebuffer::deallocate() {
    for(auto& t : attachments_) t->deallocate();
    glDeleteFramebuffers(1, &framebuffer_);
    LOG("Framebuffer Deallocated:", framebuffer_);
    framebuffer_ = INVALID_IDX;
    gl_error_check();
}

void framebuffer::bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
    colour_target_count_ == 0 ? glDrawBuffer(GL_NONE) : glDrawBuffers(uint32_t(colour_target_count_), draw_buffers.data());
    glGetIntegerv(GL_VIEWPORT, curr_viewport_.data());
    glGetDoublev(GL_DEPTH_RANGE, curr_depthrange_.data());
    glViewport(0, 0, uint32_t(width_), uint32_t(height_));
    glDepthRange(0., 1.);
}

void framebuffer::release() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if(mipmaps_ && get_colour_attachment() == attachment_type::AT_TEXTUREBUFFER) {
        for(size_t i = 0; i != colour_target_count_; ++i) {
            dynamic_cast<texture*>(attachments_[i].get())->bind();
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        dynamic_cast<texture*>(attachments_[colour_target_count_-1].get())->bind();
    }

    glViewport(curr_viewport_[0], curr_viewport_[1], curr_viewport_[2], curr_viewport_[3]);
    glDepthRange(curr_depthrange_[0], curr_depthrange_[1]);
}

bool framebuffer::is_bound() const {
    return false;
}

void framebuffer::set_colour_target(attachment_type at, pixel_format pf, pixel_datatype dt) {
    col_attachment_ = at;
    pf_col_ = pf;
    dt_col_ = dt;
}

void framebuffer::set_depth_stencil_target(attachment_type at, pixel_format pf, pixel_datatype dt) {
    ds_attachment_ = at;
    pf_ds_ = pf;
    dt_ds_ = dt;
}

bool framebuffer::initialise(size_t target_count, size_t width, size_t height, pixel_format format,
                             pixel_datatype datatype, bool mipmaps, bool depthstencil) {
    colour_target_count_ = target_count;
    width_ = width;
    height_ = height;
    pf_col_ = format;
    dt_col_ = datatype;
    mipmaps_ = mipmaps;
    col_attachment_ = target_count == 0 ? attachment_type::AT_NONE : attachment_type::AT_TEXTUREBUFFER;
    if(depthstencil) {
        pf_ds_ = pixel_format::PF_DEPTH_STENCIL;
        dt_ds_ = pixel_datatype::PD_UNSIGNED_INT_24_8;
        ftype_ = target_count > 0 ? framebuffer_type::FT_COLOUR_DEPTH_STENCIL : framebuffer_type::FT_DEPTH_STENCIL;
        ds_attachment_ = attachment_type::AT_TEXTUREBUFFER;
    } else ftype_ = framebuffer_type::FT_COLOUR;
    return initialise();
}

bool framebuffer::initialise() {
    assert(is_allocated() && "Framebuffer is not allocated");
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
    attachments_.clear();
    attachments_.reserve(get_attachment_count());

    if(has_colour()) {
        if(col_attachment_ == attachment_type::AT_TEXTUREBUFFER) {
            for(uint32_t i = 0; i != colour_target_count_; ++i) {
                auto tex_ptr = std::make_unique<texture>();
                tex_ptr->allocate();
                tex_ptr->initialise(texture_type::TT_TEX2D, int32_t(width_), int32_t(height_), 1, pf_col_, dt_col_, false); // mipmaps are handled explicitly
                tex_ptr->bind();
                if(mipmaps_) {
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                }

                glFramebufferTexture2D(GL_FRAMEBUFFER, draw_buffers[i], GL_TEXTURE_2D, tex_ptr->resource_id(), 0);
                tex_ptr->release();
                attachments_.emplace_back(std::move(tex_ptr));
                gl_error_check();
            }
        } else if(col_attachment_ == attachment_type::AT_RENDERBUFFER) {
            for(uint32_t i = 0; i != colour_target_count_; ++i) {
                auto rb_ptr = std::make_unique<renderbuffer>();
                rb_ptr->allocate();
                rb_ptr->initialise(pf_col_, dt_col_, int32_t(width_), int32_t(height_));
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, draw_buffers[i], GL_RENDERBUFFER, rb_ptr->resource_id());
                attachments_.emplace_back(std::move(rb_ptr));
                gl_error_check();
            }
        }
    } else {
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }

    if(has_depth_stencil()) {
        uint32_t attachment = GL_NONE;
        if(has_depth() && has_stencil()) attachment = GL_DEPTH_STENCIL_ATTACHMENT;
        else if(has_depth())             attachment = GL_DEPTH_ATTACHMENT;
        else if(has_stencil())           attachment = GL_STENCIL_ATTACHMENT;

        if(ds_attachment_ == attachment_type::AT_TEXTUREBUFFER) {
            auto tex_ptr = std::make_unique<texture>();
            tex_ptr->allocate();
            tex_ptr->initialise(texture_type::TT_TEX2D, int32_t(width_), int32_t(height_), 1, pf_ds_, dt_ds_, false);
            glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, tex_ptr->resource_id(), 0);
            attachments_.emplace_back(std::move(tex_ptr));
        } else if(ds_attachment_ == attachment_type::AT_RENDERBUFFER) {
            auto rb_ptr = std::make_unique<renderbuffer>();
            rb_ptr->allocate();
            rb_ptr->initialise(pf_ds_, dt_ds_, int32_t(width_), int32_t(height_));
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, rb_ptr->resource_id());
            attachments_.emplace_back(std::move(rb_ptr));
        }
    }

    switch(glCheckFramebufferStatus(GL_FRAMEBUFFER)) {
        case GL_FRAMEBUFFER_COMPLETE:
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            LOG("Render Target Error: GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT");
            assert(false);
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            LOG("Render Target Error: GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT");
            assert(false);
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
            LOG("Render Target Error: GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER");
            assert(false);
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
            LOG("Render Target Error: GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER");
            assert(false);
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED:
            LOG("Render Target Error: GL_FRAMEBUFFER_UNSUPPORTED");
            assert(false);
            break;
        default:
            LOG("Render Target Error: Unspecified (default case)");
            assert(false);
            break;
    }

    return !gl_error_check();
}

bool framebuffer::initialise(size_t target_count, size_t width, size_t height, bool mipmaps) {
    colour_target_count_ = target_count;
    width_ = width;
    height_ = height;
    mipmaps_ = mipmaps;
    return initialise();
}

bool framebuffer::read_attachment(const vec4i& viewport, size_t idx) const {
    if(viewport[2] > (int)width() || viewport[3] > (int)height()) {
        LOG_ERR("Invalid viewport specified for framebuffer attachment copy");
        return false;
    }

    const auto& fmt = idx == colour_target_count_ ? pf_ds_ : pf_col_;
    const auto& dt = idx == colour_target_count_ ? dt_ds_ : dt_col_;

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
    glReadBuffer(GL_COLOR_ATTACHMENT0 + uint32_t(idx));
    glReadPixels(viewport[0], viewport[1], viewport[2], viewport[3], gl_type(fmt), gl_type(dt), 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return !gl_error_check();
}
