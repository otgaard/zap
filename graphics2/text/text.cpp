/* Created by Darren Otgaar on 2017/09/22. http://www.github.com/otgaard/zap */

#if defined(FOUND_FREETYPE)
#include "text.hpp"
#include "text_batcher.hpp"

using namespace zap;
using namespace engine;

zap::graphics::text::text(uint32_t id, zap::graphics::text_batcher* parent) : id_(id), parent_(parent) {
}

zap::graphics::text::~text() {
    parent_->destroy_text(id_);
}

void zap::graphics::text::set_text(const std::string& str, size_t max_len) {
    parent_->change_text(id_, str);
}

zap::graphics::font* zap::graphics::text::get_font() const {
    return parent_->get_text_font(id_);
}

const std::string& zap::graphics::text::get_text() const {
    return parent_->get_text_string(id_);
}

size_t zap::graphics::text::size() const {
    return parent_->get_text_size(id_);
}


#endif //defined(FOUND_FREETYPE)
