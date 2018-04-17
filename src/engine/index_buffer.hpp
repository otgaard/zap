/* Created by Darren Otgaar on 2016/04/06. http://www.github.com/otgaard/zap */
#ifndef ZAP_INDEX_BUFFER_HPP
#define ZAP_INDEX_BUFFER_HPP

#include "buffer.hpp"

namespace zap { namespace engine {

template <typename T>
class index_buffer : public buffer {
public:
    using type = T;
    using index_t = T;
    constexpr static auto buf_type = buffer_type::BT_ELEMENT_ARRAY;

    explicit index_buffer(buffer_usage use=buffer_usage::BU_STATIC_DRAW) : buffer(use) { }
    index_buffer(const index_buffer&) = delete;
    index_buffer(index_buffer&& rhs) noexcept : buffer(std::move(rhs)), index_count_(rhs.index_count_) { }
    virtual ~index_buffer() = default;

    index_buffer& operator=(const index_buffer&) = delete;
    index_buffer& operator=(index_buffer&& rhs) noexcept {
        if(this != &rhs) {
            buffer::operator=(std::move(rhs));
            index_count_ = rhs.index_count_;
        }
        return *this;
    }

    void bind() const { buffer::bind(buf_type); }
    void release() const { buffer::release(buf_type); }

    bool initialise(size_t index_count, const char* data=nullptr) {
        if(buffer::initialise(buf_type, usage(), index_count*sizeof(T), data)) {
            index_count_ = index_count;
            return true;
        }
        return false;
    }

    bool initialise(const std::vector<char>& data) {
        if(buffer::initialise(buf_type, usage(), data)) {
            index_count_ = data.size() / sizeof(T);
            return true;
        }
        return false;
    }

    bool initialise(const std::vector<T>& data) {
        auto result = buffer::initialise(buf_type, usage(), data.size()*sizeof(T),
                                         reinterpret_cast<const char*>(data.data()));
        index_count_ = data.size();
        return result;
    }

    bool orphan() { return buffer::orphan(buf_type, usage()); }

    bool copy(const std::vector<index_t>& data, size_t offset, size_t index_count) {
        return buffer::copy(buf_type,
                            sizeof(index_t)*offset,
                            sizeof(index_t)*index_count,
                            reinterpret_cast<const char*>(data.data()));
    }

    char* map(buffer_access access) { return buffer::map(buf_type, access); }
    char* map(range_access::code access, size_t offset, size_t length) {
        assert(offset < index_count_ && offset+length <= index_count_ && ZERR_IDX_OUT_OF_RANGE);
        return buffer::map(buf_type, access, offset*sizeof(T), length*sizeof(T));
    }
    char* map(int access, size_t offset, size_t length) { return map((range_access::code)access, offset, length); }
    void flush(size_t offset, size_t length) { return buffer::flush(buf_type, offset*sizeof(T), length*sizeof(T)); }
    bool unmap() { return buffer::unmap(buf_type); }

    void mapped_copy(const index_t* ptr, size_t trg_offset, size_t index_count) {
        assert(is_mapped() && trg_offset + index_count <= index_count_ && ZERR_IDX_OUT_OF_RANGE);
        memcpy(data()+trg_offset, ptr, sizeof(index_t)*index_count);
    }
    void mapped_copy(const std::vector<index_t>& arr, size_t trg_offset, size_t index_count) {
        return mapped_copy(arr.data(), trg_offset, index_count);
    }

    const T& operator[](size_t idx) const {
        assert(is_mapped() && "Index Buffer must be mapped!");
        assert(idx < index_count_ && ZERR_IDX_OUT_OF_RANGE);
        return *(reinterpret_cast<T*>(mapped_ptr_) + idx);
    }

    T& operator[](size_t idx) {
        assert(is_mapped() && "Index Buffer must be mapped!");
        assert(idx < index_count_ && ZERR_IDX_OUT_OF_RANGE);
        return *(reinterpret_cast<T*>(mapped_ptr_) + idx);
    }

    const size_t index_count() const { return index_count_; }
    const size_t count() const { return index_count_; }

    index_t* data() {
        assert(is_mapped() && "Index Buffer must be mapped!");
        return reinterpret_cast<index_t*>(mapped_ptr_);
    }
protected:
    size_t index_count_ = 0;
};

template <typename Parm> struct is_index_buffer : std::false_type { };
template <typename T> struct is_index_buffer<index_buffer<T>> : std::true_type { };

}}

#endif //ZAP_INDEX_BUFFER_HPP
