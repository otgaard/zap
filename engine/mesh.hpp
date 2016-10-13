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
    ~mesh_base() { if(is_allocated()) deallocate(); vao_ = INVALID_RESOURCE; }

    bool is_allocated() const { return vao_ != INVALID_RESOURCE; }

    bool allocate();
    void deallocate();

    void bind() const;
    void release() const;

    void draw_arrays_impl(primitive_type type, size_t first, size_t count);
    void draw_elements_impl(primitive_type type, data_type index_type, size_t count);

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
struct mesh : public mesh_base {
public:
    static_assert(is_index_buffer<Index>::value, "Index must be of type index_buffer<>");
    using vertex_stream_t = VtxStream;
    using index_buffer_t = Index;
    constexpr static primitive_type primitive = Index::primitive;

    mesh() : mesh_base(), idx_buffer_ptr(nullptr) { }
    mesh(const vertex_stream_t& vtxstream, index_buffer_t* idx_ptr) : mesh_base(),
                                                                      vstream(vtxstream), idx_buffer_ptr(idx_ptr) { }

    void set_stream(const vertex_stream_t& vtxstream) { vstream = vtxstream; }
    void set_index(index_buffer_t* idx_ptr) { idx_buffer_ptr = idx_ptr; }
    size_t vertex_count() const { return vstream.ptr ? vstream.ptr->vertex_count() : 0; }

    void draw(size_t start=0, size_t count=0) {
        if(!idx_buffer_ptr) { LOG_ERR("No index specified"); return; }

        mesh_base::draw_elements_impl(primitive,
                                      (data_type)dt_descriptor<typename index_buffer_t::type>::value,
                                      idx_buffer_ptr->index_count());
    }

    vertex_stream_t vstream;
    Index* idx_buffer_ptr;
};

template <typename VtxStream, primitive_type Primitive>
struct mesh<VtxStream, Primitive, void> : public mesh_base {
public:
    using vertex_stream_t = VtxStream;
    constexpr static primitive_type primitive = Primitive;

    mesh() : mesh_base(), vstream(nullptr) { }
    mesh(const vertex_stream_t& vtxstream) : mesh_base(), vstream(vtxstream) { }

    void set_stream(const vertex_stream_t& vtxstream) { vstream = vtxstream; }

    size_t vertex_count() const { return vstream.ptr ? vstream.ptr->vertex_count() : 0; }

    void draw(primitive_type prim=primitive, size_t start=0, size_t count=0) {
        assert(vstream.ptr && "No vertex stream has been set for mesh draw");
        mesh_base::draw_arrays_impl(prim, start, count == 0 ? vstream.ptr->vertex_count() : count);
    }

    vertex_stream_t vstream;
};

}}

#endif //ZAP_MESH_HPP
