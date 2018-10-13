//
// Created by otgaard on 2018/10/13.
//

#pragma once

#include <vector>

namespace zap { namespace renderer { namespace engine {

template <typename IndexT>
class index_buffer : public buffer {
public:
    using index_t = IndexT;

    index_buffer() = default;
    explicit index_buffer(size_t reserve) { indices_.reserve(reserve); }

    const index_t& operator[](size_t idx) const {
        assert(idx < indices_.size() && "Out-of-bounds");
        return indices_[idx];
    }

    index_t& operator[](size_t idx) {
        assert(idx < indices_.size() && "Out-of-bounds");
        return indices_[idx];
    }

protected:

private:
    std::vector<IndexT> indices_;
};


}}}