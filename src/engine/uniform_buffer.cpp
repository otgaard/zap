//
// Created by otgaard on 2018/02/11.
//

#include "uniform_buffer.hpp"

using namespace zap::engine;

void ubuffer_base::bind() const { buffer::bind(buf_type); }
void ubuffer_base::release() const { buffer::release(buf_type); }
void ubuffer_base::bind_point(int location) const { gl::bind_buffer_base(buf_type, location, id_); }

