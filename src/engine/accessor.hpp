//
// Created by Darren Otgaar on 2018/04/13.
//

#ifndef ZAP_ACCESSOR_HPP
#define ZAP_ACCESSOR_HPP

#include <list>
#include <vector>
#include <core/core.hpp>

// The accessor provides both an allocator and accessor wrapper to buffer objects.  This allows the buffer to be
// mapped using glMapBufferRange() to use optimised flushing and also, the class provides a free-list for batch allocation.

namespace zap { namespace engine {

struct range {
    uint32_t start;
    uint32_t count;

    bool is_valid() const { return start != uint32_t(-1); }
    range() : start(uint32_t(-1)), count(0) { }
    range(uint32_t start, uint32_t count) : start(start), count(count) { }
};

template <typename BufferT>
class accessor {
public:
    using buffer_t = BufferT;
    using type = typename buffer_t::type;

    using freelist_t = std::list<range>;
    using flushlist_t = std::vector<range>;

    explicit accessor(BufferT* buffer_ptr);
    ~accessor() = default;

    uint32_t capacity() const;

    range allocate(uint32_t count);
    bool release(const range& blk);

    // Map the whole buffer
    bool map_read();
    bool map_write(bool invalidate=true);
    bool map_readwrite();

    // Map a range
    bool map_read(const range& blk);
    bool map_write(const range& blk, bool invalidate=true);
    bool map_readwrite(const range& blk);

    bool unmap();

protected:

private:
    buffer_t* buffer_ptr_;
    freelist_t freelist_;
    flushlist_t flushlist_;
    uint32_t map_start_ = INVALID_IDX;
    uint32_t map_count_ = 0;
};

template <typename BufferT>
accessor<BufferT>::accessor(BufferT *buffer_ptr) : buffer_ptr_(buffer_ptr) {
}

template <typename BufferT>
range accessor<BufferT>::allocate(uint32_t count) {
    auto it = std::find_if(freelist_.begin(), freelist_.end(), [count](const auto& blk) { return count <= blk.count; });
    if(it == freelist_.end()) return range(INVALID_IDX, 0);
    else {
        auto& curr = *it;
        const auto start = curr.start;
        if(curr.count == count) freelist_.erase(it);
        else {
            curr.start += count;
            curr.count -= count;
        }
        return range(start, count);
    }
}

}}

#endif //ZAP_ACCESSOR_HPP
