//
// Created by Darren Otgaar on 2018/04/15.
//

#ifndef ZAP_RENDER_BATCH_HPP
#define ZAP_RENDER_BATCH_HPP

#include <array>
#include <engine/index_buffer.hpp>
#include <engine/vertex_buffer.hpp>
#include <engine/accessor.hpp>
#include <engine/mesh.hpp>

namespace zap { namespace renderer {

template <typename VertexStreamT, typename IndexBufferT=void>
class render_batch;

template <typename VertexStreamT>
class render_batch<VertexStreamT, void> {
public:
    using stream_t = VertexStreamT;
    using mesh_t = engine::mesh<VertexStreamT>;
    static constexpr size_t vbuf_count = VertexStreamT::count;

protected:

private:
    std::array<engine::buffer*, vbuf_count> vbufs_;

};

template <typename VertexStreamT, typename IndexBufferT>
class render_batch {
public:
    using stream_t = VertexStreamT;
    using ibuf_t = IndexBufferT;
    using mesh_t = engine::mesh<VertexStreamT, typename IndexBufferT::type>;
    static constexpr size_t vbuf_count = VertexStreamT::count;

    render_batch() = default;

    bool initialise(const stream_t& stream, IndexBufferT* ibuf_ptr) {
        stream_ = stream;
        ibuf_ = ibuf_ptr;
        stream_.append(vbufs_.data());
        return true;
    }

protected:

private:
    stream_t stream_;
    ibuf_t* ibuf_;
    std::array<engine::buffer*, vbuf_count> vbufs_;

};

}}

#endif //ZAP_RENDER_BATCH_HPP
