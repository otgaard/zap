/* Created by Darren Otgaar on 2016/03/05. http://www.github.com/otgaard/zap */
#define LOGGING_ENABLED
#include <maths/io.hpp>
#include <tools/log.hpp>
#include <third_party/include/GL/glew.h>
#include <map>
#include "apps/application.hpp"
#include "apps/graphic_types.hpp"
#include "generators/geometry/geometry3.hpp"
#include "simple_shdr.hpp"

using namespace zap;
using namespace zap::maths;
using namespace zap::generators;

using vertex_t = vertex<pos3f_t>;
using vbuf_t = vertex_buffer<vertex_t, buffer_usage::BU_STATIC_DRAW>;
using ibuf_t = index_buffer<uint16_t, primitive_type::PT_TRIANGLES, buffer_usage::BU_STATIC_DRAW>;
using mesh_t = mesh<vertex_stream<vbuf_t>, primitive_type::PT_TRIANGLES, ibuf_t>;

class zap_example : public application {
public:
    zap_example() : application("zap_example", 1280, 768, false) { }

    bool initialise() override final;
    void update(double t, float dt) override final;
    void draw() override final;
    void shutdown() override final;

    void on_resize(int width, int height) override final;
    void on_mousemove(double x, double y) override final;
    void on_mousewheel(double xinc, double yinc) override final;

protected:
    void make_isosphere(int subdivision_levels);

    vbuf_t ico_vb_;
    ibuf_t ico_ib_;
    mesh_t ico_mesh_;

    simple_shdr shdr_;
};

bool zap_example::initialise() {
    if(!ico_vb_.allocate() || !ico_ib_.allocate() || !ico_mesh_.allocate()) {
        LOG_ERR("Couldn't allocate stuff");
        return false;
    }

    ico_mesh_.set_stream(&ico_vb_); ico_mesh_.set_index(&ico_ib_);
    make_isosphere(1);

    shdr_.initialise();
    auto proj = make_perspective(120.f, 1280/768.f, .5f, 100.f);
    auto mv = make_frame(vec3f(0,0,-1), vec3f(0,1,0), vec3f(0,0,-5));
    auto rot = make_rotation(vec3f(0,1,0), 0.3f);
    shdr_.pvm_matrix(proj*mv*rot);

    return true;
}

void zap_example::on_resize(int width, int height) {
}

void zap_example::on_mousemove(double x, double y) {
}

void zap_example::on_mousewheel(double xinc, double yinc) {
}

float inc = 0.f;

void zap_example::update(double t, float dt) {
    auto proj = make_perspective(45.f, 1280/768.f, .5f, 100.f);
    auto mv = make_frame(vec3f(0,0,-1), vec3f(0,1,0), vec3f(0,0,-5));
    auto rot = make_rotation(vec3f(0,1,0), inc);
    shdr_.pvm_matrix(proj*mv*rot);
    inc += 0.01f;
}

void zap_example::draw() {
    shdr_.bind();
    ico_mesh_.bind();
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    bf_culling(false);
    ico_mesh_.draw();
    ico_mesh_.release();
    shdr_.release();
}

void zap_example::shutdown() {
}

int main(int argc, char* argv[]) {
    zap_example app;
    return app.run();
}

void zap_example::make_isosphere(int subdivision_levels) {
    using edge_midpoint_tbl = std::map<std::tuple<int,int>, int>;
    auto ico = generators::geometry3<vertex_t,primitive_type::PT_TRIANGLES>::make_icosahedron<float>();

    std::vector<vertex_t> vbuf = std::get<0>(ico);
    std::vector<uint16_t> ibuf = std::get<1>(ico);
    vbuf.reserve(100000);

    edge_midpoint_tbl midpoints;

    auto get_centre = [&vbuf, &midpoints](int p1, int p2)->int {
        auto key = p1 < p2 ? std::make_tuple(p1,p2) : std::make_tuple(p2,p1);
        auto it = midpoints.find(key);
        if(it != midpoints.end()) return it->second;

        auto& pnt1 = vbuf[p1].position;
        auto& pnt2 = vbuf[p2].position;

        int idx = int(vbuf.size());
        vbuf.push_back({{normalise(.5f*(pnt1+pnt2))}});
        midpoints.insert(std::make_pair(std::make_tuple(p1,p2), idx));
        return idx;
    };

    for(size_t sub = 0; sub != subdivision_levels; ++sub) {
        std::vector<uint16_t> new_ibuf;
        new_ibuf.reserve(ibuf.size()*4*3);

        for(size_t idx = 0, iend = ibuf.size()/3; idx != iend; ++idx) {
            int offset = 3*idx, oA = ibuf[offset], oB = ibuf[offset+1], oC = ibuf[offset+2];
            int nA = get_centre(oA,oB), nB = get_centre(oB,oC), nC = get_centre(oC,oA);
            new_ibuf.push_back(oA); new_ibuf.push_back(nA), new_ibuf.push_back(nC);
            new_ibuf.push_back(oB); new_ibuf.push_back(nB), new_ibuf.push_back(nA);
            new_ibuf.push_back(oC); new_ibuf.push_back(nC), new_ibuf.push_back(nB);
            new_ibuf.push_back(nA); new_ibuf.push_back(nB), new_ibuf.push_back(nC);
        }

        ibuf = new_ibuf;
    }

    ico_mesh_.bind();
    ico_vb_.bind();
    ico_vb_.initialise(vbuf);
    ico_ib_.bind();
    ico_ib_.initialise(ibuf);
    ico_mesh_.release();
}
