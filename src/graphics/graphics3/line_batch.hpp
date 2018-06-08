/* Created by Darren Otgaar on 2018/04/22. http://www.github.com/otgaard/zap */
#ifndef ZAP_LINE_BATCH_HPP
#define ZAP_LINE_BATCH_HPP

#include <graphics/graphics3/g3_types.hpp>
#include <renderer/render_batch.hpp>
#include <engine/program.hpp>

namespace zap { namespace graphics {

class line_batch {
public:
    using rndr_batch_t = renderer::render_batch<vertex_stream<vbuf_p3tn3c4t2ps1_t>, ibuf_u32_t>;
    using vertex_t = vtx_p3tn3c4t2ps1_t;
    using vbuf_t = vbuf_p3tn3c4t2ps1_t;
    using index_t = uint32_t;
    using ibuf_t = ibuf_u32_t;
    using mesh_t = mesh_p3tn3c4t2ps1_u32_t;

    using vec2f = maths::vec2f;
    using vec3f = maths::vec3f;
    using vec4b = maths::vec4b;

    line_batch();
    ~line_batch();

    bool initialise(uint32_t line_count);       // In segments, i.e 4 vertices, 6 indices

    bool is_mapped() const { return batch_.is_mapped(); }

    uint32_t create_line(const vec3f& A, const vec3f& B, const vec4b& colour, float width);
    uint32_t create_line(const std::vector<vec3f>& points, const vec4b& colour, float width);
    bool update_line(uint32_t id, const vec3f& A, const vec3f& B, const vec4b& colour, float width);
    bool update_line(uint32_t id, const std::vector<vec3f>& points, const vec4b& colour, float width);

    void draw(uint32_t id, const mat4f& MVP);

protected:

private:
    rndr_batch_t batch_;
    std::vector<rndr_batch_t::token> segments_;
    std::vector<rndr_batch_t::token> strips_;
    program prog_;
    uint32_t uniforms_[3];
};

}}

#endif //ZAP_LINE_BATCH_HPP
