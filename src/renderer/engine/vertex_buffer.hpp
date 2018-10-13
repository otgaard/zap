//
// Created by otgaard on 2018/10/13.
//

#pragma once

#include <vector>

namespace zap { namespace renderer { namespace engine {

template <typename VertexT>
class vertex_buffer : public buffer {
public:
    using vertex_t = VertexT;

    vertex_buffer() = default;
    explicit vertex_buffer(size_t reserve) { vertices_.reserve(reserve); }
    ~vertex_buffer() = default;

    const vertex_t& operator[](size_t idx) const {
        assert(idx < vertices_.size() && "Out-of-bounds");
        return vertices_[idx];
    }

    vertex_t& operator[](size_t idx) {
        assert(idx < vertices_.size() && "Out-of-bounds");
        return vertices_[idx];
    }
protected:

private:
    std::vector<VertexT> vertices_;
};

}}}
