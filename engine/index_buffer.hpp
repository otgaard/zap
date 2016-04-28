/* Created by Darren Otgaar on 2016/04/06. http://www.github.com/otgaard/zap */
#ifndef ZAP_INDEX_BUFFER_HPP
#define ZAP_INDEX_BUFFER_HPP

#include "buffer.hpp"

namespace zap { namespace engine {

template <typename T, primitive_type PRIM, buffer_usage USAGE>
class index_buffer : public buffer {
public:
    using type = T;
    constexpr static auto primitive = PRIM;
    constexpr static auto buf_type = buffer_type::BT_ELEMENT_ARRAY;
    constexpr static auto usage = USAGE;

    index_buffer() : index_count_(0) { }

    void bind() const { buffer::bind(buf_type); }
    void release() const { buffer::release(buf_type); }

    bool initialise(size_t index_count, const char* data=nullptr) {
        if(buffer::initialise(buf_type, usage, index_count*sizeof(T), data)) {
            index_count_ = index_count;
            return true;
        }
        return false;
    }

    bool initialise(const std::vector<char>& data) {
        if(buffer::initialise(buf_type, usage, data)) {
            index_count_ = data.size() / sizeof(T);
            return true;
        }
        return false;
    }

    bool initialise(const std::vector<T>& data) {
        auto result = buffer::initialise(buf_type, usage, data.size()*sizeof(T),
                                         reinterpret_cast<const char*>(data.data()));
        index_count_ = data.size();
        return result;
    }

    char* map(buffer_access access) { return buffer::map(buf_type, access); }
    char* map(buffer_access access, size_t offset, size_t length) {
        return buffer::map(buf_type, access, offset, length);
    }
    bool unmap() { return buffer::unmap(buf_type); }

    const T& operator[](size_t idx) const {
        assert(is_mapped() && "Index Buffer must be mapped!");
        assert(idx < index_count_ && ZERR_IDX_OUT_OF_RANGE);
        return *(reinterpret_cast<T*>(mapped_ptr_) + idx);
    }

    const size_t index_count() const { return index_count_; }

protected:
    size_t index_count_;
};

}}

#endif //ZAP_INDEX_BUFFER_HPP
