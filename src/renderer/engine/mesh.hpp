//
// Created by otgaard on 2018/10/13.
//

#pragma once

namespace zap { namespace renderer { namespace engine {

template <typename VtxBufferT, typename IdxBufferT> class mesh;

template <typename VtxBufferT, typename IdxBufferT=void>
struct mesh<VtxBufferT, void> {
    mesh() = default;
    explicit mesh(VtxBufferT* vbuffer_ptr) : vbuffer_ptr(vbuffer_ptr) { }

    VtxBufferT* vbuffer_ptr = nullptr;
};

template <typename VertexT, typename IndexT>
class mesh<VertexT, IndexT> {
    mesh() = default;
    explicit mesh(VtxBufferT* vbuffer_ptr, IdxBufferT* ibuffer_ptr) : vbuffer_ptr(vbuffer_ptr), ibuffer_ptr(ibuffer_ptr) { }

    VtxBufferT* vbuffer_ptr = nullptr;
    IdxBufferT* ibuffer_ptr = nullptr;
};

}}}