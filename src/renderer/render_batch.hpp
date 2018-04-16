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

template <typename VertexStreamT, typename IndexBufferT=void, size_t Count=VertexStreamT::count>
class render_batch;

// Note: The render_batch should expose an interface for the entire stream but variadic arrays make the interface
// cumbersome. For now, these templates are explicitly specialised and using the VertexStream::count as a hack.

template <typename VertexStreamT>
class render_batch<VertexStreamT, void, 1> {
public:
    using stream_t = VertexStreamT;
    static constexpr size_t count = 1;
    using vbuf_t = typename engine::stream_query<0, stream_t>::type;
    using vertex_t = typename vbuf_t::type;
    using mesh_t = engine::mesh<VertexStreamT>;
    using vbuf_acc_t = engine::accessor<vbuf_t>;
    using range = engine::range;
    using primitive_type = engine::primitive_type;

    struct token {
        uint32_t id;
        primitive_type type;
        range vtx_range;

        operator bool() const { return is_valid(); }
        bool is_valid() const { return id != INVALID_IDX; }
        void clear() { id = INVALID_IDX; }

        token() = default;
        token(uint32_t id, primitive_type pt, const range& vrange) : id(id), type(pt), vtx_range(vrange) { }
    };

    static const token invalid_token;

    render_batch() = default;
    ~render_batch() = default;

    const token& get_token(uint32_t id) const {
        return id < batch_.size() && batch_[id].is_valid() ? batch_[id] : invalid_token;
    }

    bool initialise(uint32_t vertex_count, engine::buffer_usage usage=engine::buffer_usage::BU_STATIC_DRAW) {
        mesh_ = engine::make_mesh<vertex_t>(size_t(vertex_count), usage);
        if(!mesh_.is_allocated()) return false;

        vbuf_ptr_ = mesh_.vstream.ptr;
        vbuf_acc_.initialise(vbuf_ptr_);
        return true;
    }

    token allocate(primitive_type type, uint32_t count) {
        auto rng = vbuf_acc_.allocate(count);
        if(rng.is_valid()) {
            uint32_t id = search_free_ ? find_free() : INVALID_IDX;
            if(id == INVALID_IDX) {
                id = uint32_t(batch_.size());
                batch_.push_back(token(id, type, rng));
            } else {
                batch_[id] = token(id, type, rng);
            }

            return batch_[id];
        }

        return invalid_token;
    }

    void bind() { mesh_.bind(); }
    void release() { mesh_.release(); }

    bool map_read() { return vbuf_acc_.map_read(); }
    bool map_write() { return vbuf_acc_.map_write(); }
    bool map_readwrite() { return vbuf_acc_.map_readwrite(); }

    bool map_read(const token& tok) { return vbuf_acc_.map_read(tok.vtx_range); }
    bool map_write(const token& tok) { return vbuf_acc_.map_write(tok.vtx_range); }
    bool map_readwrite(const token tok) { return vbuf_acc_.map_readwrite(tok.vtx_range); }

    bool unmap() { return vbuf_acc_.unmap(); }

    bool load(const token& tok, const std::vector<vertex_t>& m) {
        if(map_write(tok)) {
            set(tok, 0, m);
            return unmap();
        }

        return false;
    }

    void set(const token& tok, uint32_t offset, const vertex_t& v) { vbuf_acc_.set(tok.vtx_range, offset, v); }
    void set(const token& tok, uint32_t offset, uint32_t count, const vertex_t* p) { vbuf_acc_.set(tok.vtx_range, offset, count, p); }
    void set(const token& tok, uint32_t offset, const std::vector<vertex_t>& v) { vbuf_acc_.set(tok.vtx_range, offset, v); }

    void draw(const token& tok) { mesh_.draw(tok.type, tok.vtx_range.start, tok.vtx_range.count); }

protected:
    uint32_t find_free() const {
        auto it = std::find_if(batch_.begin(), batch_.end(), [](const auto& tk){ return !tk.is_valid(); });
        return it != batch_.end() ? uint32_t(it - batch_.begin()) : INVALID_IDX;
    }

private:
    mesh_t mesh_;
    vbuf_t* vbuf_ptr_ = nullptr;
    engine::accessor<vbuf_t> vbuf_acc_;
    std::vector<token> batch_;
    bool search_free_ = false;
};

template <typename VertexStreamT, typename IndexBufferT>
class render_batch<VertexStreamT, IndexBufferT, 1> {
public:
    using stream_t = VertexStreamT;
    static constexpr size_t count = 1;
    using vbuf_t = typename engine::stream_query<0, stream_t>::type;
    using ibuf_t = IndexBufferT;
    using vertex_t = typename vbuf_t::type;
    using index_t = typename ibuf_t::type;
    using mesh_t = engine::mesh<VertexStreamT, ibuf_t>;
    using vbuf_acc_t = engine::accessor<vbuf_t>;
    using range = engine::range;
    using primitive_type = engine::primitive_type;

    struct token {
        uint32_t id;
        primitive_type type;
        range vtx_range;
        range idx_range;

        operator bool() const { return is_valid(); }
        bool is_valid() const { return id != INVALID_IDX; }
        void clear() { id = INVALID_IDX; }

        token() = default;
        token(uint32_t id, primitive_type pt, const range& vrange, const range& irange)
                : id(id), type(pt), vtx_range(vrange), idx_range(irange) { }
    };

    static const token invalid_token;

    render_batch() = default;
    ~render_batch() = default;

    const token& get_token(uint32_t id) const {
        return id < batch_.size() && batch_[id].is_valid() ? batch_[id] : invalid_token;
    }

    bool initialise(uint32_t vertex_count, uint32_t index_count, engine::buffer_usage usage=engine::buffer_usage::BU_STATIC_DRAW) {
        mesh_ = engine::make_mesh<vertex_t, index_t>(size_t(vertex_count), size_t(index_count), usage);
        if(!mesh_.is_allocated()) return false;

        vbuf_ptr_ = mesh_.vstream.ptr;
        vbuf_acc_.initialise(vbuf_ptr_);
        ibuf_ptr_ = mesh_.idx_buffer_ptr;
        ibuf_acc_.initialise(ibuf_ptr_);
        return true;
    }

    token allocate(primitive_type type, uint32_t vcount, uint32_t icount) {
        auto vrng = vbuf_acc_.allocate(vcount);
        auto irng = ibuf_acc_.allocate(icount);
        if(vrng.is_valid() && irng.is_valid()) {
            uint32_t id = search_free_ ? find_free() : INVALID_IDX;
            if(id == INVALID_IDX) {
                id = uint32_t(batch_.size());
                batch_.push_back(token(id, type, vrng, irng));
            } else {
                batch_[id] = token(id, type, vrng, irng);
            }

            return batch_[id];
        }

        return invalid_token;
    }

    void bind() { mesh_.bind(); }
    void release() { mesh_.release(); }

    bool map_read(bool vbuf=true, bool ibuf=true) { return (vbuf ? vbuf_acc_.map_read() : true) && (ibuf ? ibuf_acc_.map_read() : true); }
    bool map_write(bool vbuf=true, bool ibuf=true) { return (vbuf ? vbuf_acc_.map_write() : true) && (ibuf ? ibuf_acc_.map_write() : true); }
    bool map_readwrite(bool vbuf=true, bool ibuf=true) { return (vbuf ? vbuf_acc_.map_readwrite() : true) && (ibuf ? ibuf_acc_.map_readwrite() : true); }

    bool map_read(const token& tok, bool vbuf=true, bool ibuf=true) { return (vbuf ? vbuf_acc_.map_read(tok.vtx_range) : true) && (ibuf ? ibuf_acc_.map_read(tok.idx_range) : true); }
    bool map_write(const token& tok, bool vbuf=true, bool ibuf=true) { return (vbuf ? vbuf_acc_.map_write(tok.vtx_range) : true) && (ibuf ? ibuf_acc_.map_write(tok.idx_range) : true); }
    bool map_readwrite(const token tok, bool vbuf=true, bool ibuf=true) { return (vbuf ? vbuf_acc_.map_readwrite(tok.vtx_range) : true) && (ibuf ? ibuf_acc_.map_readwrite(tok.idx_range) : true); }

    bool unmap() { return (vbuf_acc_.is_mapped() ? vbuf_acc_.unmap() : true) && (ibuf_acc_.is_mapped() ? ibuf_acc_.unmap() : true); }

    bool load(const token& tok, const std::vector<vertex_t>& v) {
        if(map_write(tok, true, false)) {
            set(tok, 0, v);
            return unmap();
        }

        return false;
    }

    bool load(const token& tok, const std::pair<std::vector<vertex_t>, std::vector<uint32_t>>& m, bool remap=true) {
        if(map_write(tok)) {
            set(tok, 0, 0, m, remap);
            return unmap();
        }

        return false;
    }

    void set(const token& tok, uint32_t offset, const vertex_t& v) { vbuf_acc_.set(tok.vtx_range, offset, v); }
    void set(const token& tok, uint32_t offset, uint32_t count, const vertex_t* p) { vbuf_acc_.set(tok.vtx_range, offset, count, p); }
    void set(const token& tok, uint32_t offset, const std::vector<vertex_t>& v) { vbuf_acc_.set(tok.vtx_range, offset, v); }

    void set(const token& tok, uint32_t offset, const index_t& v, bool remap=true) {
        ibuf_acc_.set(tok.idx_range, remap ? tok.vtx_range.start + offset : offset, v);
    }

    void set(const token& tok, uint32_t offset, uint32_t count, const index_t* p, bool remap=true) {
        if(remap) {
            std::vector<index_t> arr(count);
            const auto start = tok.vtx_range.start;
            std::transform(p, p+count, arr.begin(), [start](auto& v) { return v + start; });
            ibuf_acc_.set(tok.idx_range, offset, arr);
        } else {
            ibuf_acc_.set(tok.idx_range, offset, count, p);
        }
    }

    void set(const token& tok, uint32_t offset, const std::vector<index_t>& v, bool remap=true) {
        if(remap) {
            std::vector<index_t> arr(v.size());
            const auto start = tok.vtx_range.start;
            std::transform(v.begin(), v.end(), arr.begin(), [start](auto& v) { return v + start; });
            ibuf_acc_.set(tok.idx_range, offset, arr);
        } else {
            ibuf_acc_.set(tok.idx_range, offset, v);
        }
    }

    void set(const token& tok, uint32_t voffset, uint32_t ioffset, const std::pair<std::vector<vertex_t>, std::vector<index_t>>& m, bool remap=true) {
        set(tok, voffset, m.first);
        set(tok, ioffset, m.second, remap);
    }

    void draw(const token& tok) { mesh_.draw(tok.type, tok.idx_range.start, tok.idx_range.count); }

protected:
    uint32_t find_free() const {
        auto it = std::find_if(batch_.begin(), batch_.end(), [](const auto& tk){ return !tk.is_valid(); });
        return it != batch_.end() ? uint32_t(it - batch_.begin()) : INVALID_IDX;
    }

private:
    mesh_t mesh_;
    vbuf_t* vbuf_ptr_ = nullptr;
    ibuf_t* ibuf_ptr_ = nullptr;
    engine::accessor<vbuf_t> vbuf_acc_;
    engine::accessor<ibuf_t> ibuf_acc_;
    std::vector<token> batch_;
    bool search_free_ = false;
};



template <typename VertexStreamT>
const typename render_batch<VertexStreamT, void, 1>::token render_batch<VertexStreamT, void, 1>::invalid_token = {INVALID_IDX, engine::primitive_type::PT_NONE, engine::range()};

template <typename VertexStreamT, typename IndexBufferT>
const typename render_batch<VertexStreamT, IndexBufferT, 1>::token render_batch<VertexStreamT, IndexBufferT, 1>::invalid_token = {INVALID_IDX, engine::primitive_type::PT_NONE, engine::range(), engine::range()};

}}

#endif //ZAP_RENDER_BATCH_HPP
