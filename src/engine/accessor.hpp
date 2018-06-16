//
// Created by Darren Otgaar on 2018/04/13.
//

#ifndef ZAP_ACCESSOR_HPP
#define ZAP_ACCESSOR_HPP

#include <list>
#include <vector>
#include <core/core.hpp>
#include <engine/engine.hpp>

// The accessor provides both an allocator and accessor wrapper to buffer objects.  This allows the buffer to be
// mapped using glMapBufferRange() to use optimised flushing and also, the class provides a free-list for batch allocation.

namespace zap { namespace engine {

struct range {
    uint32_t start;
    uint32_t count;

    operator bool() const { return is_valid(); }
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

    accessor() = default;
    explicit accessor(BufferT* buffer_ptr);
    ~accessor();

    bool initialise(BufferT* buffer_ptr);

    bool is_initialised() const;
    bool is_mapped() const;
    bool is_flushed() const;

    uint32_t capacity() const;
    uint32_t allocated() const;
    uint32_t available() const;

    range allocate(uint32_t count);
    void release(const range& blk);

    // Map the whole buffer
    bool map_read();
    bool map_write(bool invalidate=true);
    bool map_readwrite();

    // Map a range
    bool map_read(const range& blk);
    bool map_write(const range& blk, bool invalidate=true);
    bool map_readwrite(const range& blk);

    void flush();
    bool unmap();

    void set(uint32_t offset, const type& i);
    void set(uint32_t offset, uint32_t count, const type* p);
    void set(uint32_t offset, const std::vector<type>& v);

    void set(const range& blk, uint32_t offset, const type& i);
    void set(const range& blk, uint32_t offset, uint32_t count, const type* p);
    void set(const range& blk, uint32_t offset, const std::vector<type>& v);

    bool get(const range& blk, uint32_t offset, type& i);
    uint32_t get(const range& blk, uint32_t offset, uint32_t count, type* p);
    uint32_t get(const range& blk, uint32_t offset, uint32_t count, std::vector<type>& v);

protected:
    void enqueue_flush(const range& blk);

private:
    buffer_t* buffer_ptr_ = nullptr;
    freelist_t freelist_;
    flushlist_t flushlist_;
    uint32_t map_start_ = INVALID_IDX;
    uint32_t map_count_ = 0;
};

template <typename BufferT>
accessor<BufferT>::accessor(BufferT *buffer_ptr) {
    initialise(buffer_ptr);
}

template <typename BufferT>
accessor<BufferT>::~accessor() {
    if(is_initialised() && is_mapped()) {
        flush();
        unmap();
    }
}

template <typename BufferT>
bool accessor<BufferT>::initialise(BufferT* buffer_ptr) {
    if(buffer_ptr_) return false;

    buffer_ptr_ = buffer_ptr;
    freelist_.push_back(range(0, uint32_t(buffer_ptr_->count())));
    return true;
}

template <typename BufferT>
bool accessor<BufferT>::is_initialised() const {
    return buffer_ptr_ != nullptr;
}

template <typename BufferT>
bool accessor<BufferT>::is_mapped() const {
    return buffer_ptr_->is_mapped();
}

template <typename BufferT>
bool accessor<BufferT>::is_flushed() const {
    return flushlist_.empty();
}

template <typename BufferT>
uint32_t accessor<BufferT>::capacity() const {
    return buffer_ptr_->count();
}

template <typename BufferT>
uint32_t accessor<BufferT>::allocated() const {
    return capacity() - available();
}

template <typename BufferT>
uint32_t accessor<BufferT>::available() const {
    uint32_t count = 0;
    std::for_each(freelist_.begin(), freelist_.end(), [&count](const auto& blk) { count += blk.count; });
    return count;
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

template <typename BufferT>
void accessor<BufferT>::release(const range& blk) {
    if(freelist_.empty()) freelist_.push_back(blk);
    else {
        auto it = freelist_.begin();
        while(it->start < blk.start && it != freelist_.end())
            it = std::next(it);

        if(it == freelist_.end()) {
            if(freelist_.back().start + freelist_.back().count == blk.start) freelist_.back().count += blk.count;
            else                                                             freelist_.push_back(blk);
        }

        if(blk.start + blk.count == it->start) {
            it->start = blk.start;
            it->count += blk.count;
            if(it != freelist_.begin()) {
                auto prev = std::prev(it);
                if(prev->start + prev->count == it->start) {
                    prev->count += it->count;
                    freelist_.erase(it);
                }
            }
        } else if(it == freelist_.begin()) {
            freelist_.insert(it, blk);
        } else {
            auto prev = std::prev(it);
            if(prev->start + prev->count == blk.start) {
                prev->count += blk.count;
            } else {
                freelist_.insert(it, blk);
            }
        }
    }
}

template <typename BufferT>
bool accessor<BufferT>::map_read() {

}

template <typename BufferT>
bool accessor<BufferT>::map_write(bool invalidate) {
    const auto access = range_access::BA_MAP_WRITE | range_access::BA_MAP_FLUSH_EXPLICIT
                      | (invalidate ? range_access::BA_MAP_INVALIDATE_BUFFER : 0);
    map_start_ = 0; map_count_ = buffer_ptr_->count();
    buffer_ptr_->bind();
    bool success = buffer_ptr_->map(access, map_start_, map_count_) != nullptr;
    buffer_ptr_->release();
    return success;
}

template <typename BufferT>
bool accessor<BufferT>::map_readwrite() {

}

template <typename BufferT>
bool accessor<BufferT>::map_read(const range& blk) {

}

template <typename BufferT>
bool accessor<BufferT>::map_write(const range& blk, bool invalidate) {
    const auto access = range_access::BA_MAP_WRITE | range_access::BA_MAP_FLUSH_EXPLICIT
                        | (invalidate ? range_access::BA_MAP_INVALIDATE_RANGE : 0);
    if(blk.start + blk.count > buffer_ptr_->count()) return false;
    map_start_ = blk.start; map_count_ = blk.count;
    buffer_ptr_->bind();
    bool success = buffer_ptr_->map(access, map_start_, map_count_) != nullptr;
    buffer_ptr_->release();
    return success;
}

template <typename BufferT>
bool accessor<BufferT>::map_readwrite(const range& blk) {

}

template <typename BufferT>
void accessor<BufferT>::flush() {
    if(!flushlist_.empty()) {
        buffer_ptr_->bind();
        for(const auto& blk : flushlist_) buffer_ptr_->flush(blk.start, blk.count);
        buffer_ptr_->release();
        flushlist_.clear();
    }
}

template <typename BufferT>
bool accessor<BufferT>::unmap() {
    bool success = true;
    if(buffer_ptr_->is_mapped()) {
        if(!is_flushed()) flush();
        buffer_ptr_->bind();
        success = buffer_ptr_->unmap();
        buffer_ptr_->release();
        map_start_ = INVALID_IDX; map_count_ = 0;
    }
    return success;
}

template <typename BufferT>
void accessor<BufferT>::set(uint32_t offset, const type& i) {
    if(!is_mapped()) return;

    buffer_ptr_->operator[](offset) = i;

    enqueue_flush(range(offset, 1));
}

template <typename BufferT>
void accessor<BufferT>::set(uint32_t offset, uint32_t count, const type* p) {
    if(!is_mapped()) return;

    buffer_ptr_->mapped_copy(p, offset, count);

    enqueue_flush(range(offset, count));
}

template <typename BufferT>
void accessor<BufferT>::set(uint32_t offset, const std::vector<type>& v) {
    if(!is_mapped()) return;

    buffer_ptr_->mapped_copy(v, offset, v.size());

    enqueue_flush(range(offset, v.size()));
}

template <typename BufferT>
void accessor<BufferT>::set(const range& blk, uint32_t offset, const type& i) {
    if(!blk.is_valid() || offset >= blk.count || !is_mapped()) return;

    const auto idx = map_start_ == 0 ? blk.start + offset : offset;
    buffer_ptr_->operator[](idx) = i;

    enqueue_flush(range(idx, 1));
}

template <typename BufferT>
void accessor<BufferT>::set(const range& blk, uint32_t offset, uint32_t count, const type* p) {
    if(!blk.is_valid() || offset + count > blk.count || !is_mapped()) return;

    const uint32_t idx = map_start_ == 0 ? blk.start + offset : offset;
    buffer_ptr_->mapped_copy(p, idx, count);

    enqueue_flush(range(idx, count));
}

template <typename BufferT>
void accessor<BufferT>::set(const range& blk, uint32_t offset, const std::vector<type>& v) {
    if(!blk.is_valid() || offset + v.size() > blk.count || !is_mapped()) return;

    const uint32_t idx = map_start_ == 0 ? blk.start + offset : offset, count = uint32_t(v.size());
    buffer_ptr_->mapped_copy(v, idx, count);

    enqueue_flush(range(idx, uint32_t(v.size())));
}

template <typename BufferT>
bool accessor<BufferT>::get(const range& blk, uint32_t offset, type& i) {

}

template <typename BufferT>
uint32_t accessor<BufferT>::get(const range& blk, uint32_t offset, uint32_t count, type* p) {

}

template <typename BufferT>
uint32_t accessor<BufferT>::get(const range& blk, uint32_t offset, uint32_t count, std::vector<type>& v) {

}

template <typename BufferT>
void accessor<BufferT>::enqueue_flush(const range& blk) {
    if(is_flushed()) flushlist_.push_back(blk);
    else {
        // Optimise the sequential Use-case
        auto& back = flushlist_.back();
        if(back.start + back.count == blk.start) back.count += blk.count;
        else flushlist_.push_back(blk);
    }

}

}}

#endif //ZAP_ACCESSOR_HPP
