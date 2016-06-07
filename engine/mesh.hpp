/* Created by Darren Otgaar on 2016/04/16. http://www.github.com/otgaard/zap */
#ifndef ZAP_MESH_HPP
#define ZAP_MESH_HPP

#include "engine.hpp"
#include "index_buffer.hpp"
#include "vertex_buffer.hpp"

namespace zap { namespace engine {

class mesh_base {
public:
    mesh_base() : vao_(INVALID_RESOURCE) { }
    ~mesh_base() { vao_ = INVALID_RESOURCE; }

    void allocate();
    void bind();
    void release();

    void draw_impl(primitive_type type, size_t first, size_t count);

private:
    resource_t vao_;
};

template <typename... VBuffers>
struct vertex_stream {
};

template <typename VBuf, class... VBuffers>
struct vertex_stream<VBuf, VBuffers...> : vertex_stream<VBuffers...> {
    static_assert(is_vertex_buffer<VBuf>::value, "VBuf must be of type vertex_buffer<>");
    using type = VBuf;
    using ptr_t = VBuf*;
    vertex_stream() : ptr(nullptr) { }
    vertex_stream(VBuf* vbuf_ptr, VBuffers*... vbuffers) : vertex_stream<VBuffers...>(vbuffers...), ptr(vbuf_ptr) { }
    VBuf* ptr;
};

template <size_t k, typename... VBuffers> struct stream_query;

template <typename VBuf, typename... VBuffers>
struct stream_query<0, vertex_stream<VBuf, VBuffers...>> {
    using type = VBuf;
};

template <size_t k, typename VBuf, typename... VBuffers>
struct stream_query<k, vertex_stream<VBuf, VBuffers...>> {
    using type = typename stream_query<k-1, vertex_stream<VBuffers...>>::type;
};

template <size_t k, typename... VBuffers>
constexpr typename std::enable_if<k==0, typename stream_query<0, vertex_stream<VBuffers...>>::type>::type* get(vertex_stream<VBuffers...>& D) {
    return D.ptr;
};

template <size_t k, typename VBuf, typename... VBuffers>
constexpr typename std::enable_if<k!=0, typename stream_query<k, vertex_stream<VBuf, VBuffers...>>::type>::type* get(vertex_stream<VBuf, VBuffers...>& D) {
    vertex_stream<VBuffers...>& base = D;
    return get<k-1>(base);
};

template <typename VtxStream, primitive_type Primitive, typename Index=void>
struct mesh {
public:
    static_assert(is_index_buffer<Index>::value, "Index must be of type index_buffer<>");
    using vertex_stream = VtxStream;
    constexpr static primitive_type primitive = Index::primitive;
};

template <typename VtxStream, primitive_type Primitive>
struct mesh<VtxStream, Primitive, void> : public mesh_base {
public:
    using vertex_stream = VtxStream;
    constexpr static primitive_type primitive = Primitive;

    mesh() : mesh_base() { }
    mesh(const vertex_stream& vtxstream) : mesh_base(), vstream(vtxstream) { }

    void set_stream(const vertex_stream& vtxstream) { vstream = vtxstream; }

    size_t vertex_count() const { return vstream.ptr ? vstream.ptr->vertex_count() : 0; }

    void draw(primitive_type prim=primitive, size_t start=0, size_t count=0) {
        mesh_base::draw_impl(prim, start, count == 0 ? vstream.ptr->vertex_count() : count);
    }

    vertex_stream vstream;
};

}}

#endif //ZAP_MESH_HPP
