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

template <typename VertexStreamT>
const typename render_batch<VertexStreamT, void, 1>::token render_batch<VertexStreamT, void, 1>::invalid_token = {INVALID_IDX, engine::primitive_type::PT_NONE, engine::range()};

}}

#endif //ZAP_RENDER_BATCH_HPP
