/* Created by Darren Otgaar on 2016/03/05. http://www.github.com/otgaard/zap */
#define LOGGING_ENABLED
#include <maths/io.hpp>
#include <tools/log.hpp>

#include "quad.hpp"
#include "apps/application.hpp"
#include "simple_shdr.hpp"
#include <graphics2/plotter2.hpp>
#include <maths/curves/hermite.hpp>
#include <generators/textures/planar.hpp>
#include <maths/rand_lcg.hpp>

using namespace zap;
using namespace zap::maths;
using namespace zap::engine;
using namespace zap::renderer;

bool is_contained(const std::vector<vec2f>& hull, const vec2f& P) {
    const size_t hull_size = hull.size();
    for(size_t i = 0; i != hull_size; ++i) {
        if(dotperp(P - hull[i], hull[(i+1)%hull_size] - hull[i]) < 0.f) return false;
    }
    return true;
}

std::vector<vec2f> convex_hull(const std::vector<vec2f>& input) {
    std::vector<size_t> edges;

    const size_t samples = input.size();
    for(size_t p = 0; p != samples; ++p) {
        for(size_t q = 0; q != samples; ++q) {
            if(p == q) continue;
            bool is_hull_edge = true;
            vec2f E = input[q] - input[p];
            for(size_t r = 0; r < samples; ++r) {
                if(r != p && r != q) {
                    if(dotperp(input[r] - input[p], E) < 0.f) {
                        is_hull_edge = false;
                        break;
                    }
                }
            }

            if(is_hull_edge) {
                edges.push_back(p);
                edges.push_back(q);
            }
        }
    }

    std::vector<vec2f> hull;
    hull.push_back(input[edges[0]]);
    hull.push_back(input[edges[1]]);

    const vec2f* first = &input[edges[0]];
    const vec2f* last =  &input[edges[1]];
    while(last != first) {
        for(size_t i = 0; i != edges.size(); i += 2) {
            if(last == &input[edges[i]]) {
                if(&input[edges[i+1]] == first) last = &input[edges[i+1]];
                else {
                    hull.push_back(input[edges[i+1]]);
                    last = &input[edges[i+1]];
                }
                break;
            }
        }
    }

    return hull;
}

using mesh_p2_points_t = mesh<vertex_stream<vbuf_p2_t>, primitive_type::PT_POINTS>;

class zap_example : public application {
public:
    zap_example() : application("zap_example", 1280, 768, false) { }

    void initialise() override final;
    void update(double t, float dt) override final;
    void draw() override final;
    void shutdown() override final;

    void on_resize(int width, int height) override final;
    void on_mousemove(double x, double y) override final;
    void on_mousewheel(double xinc, double yinc) override final;

protected:
    graphics2::plotter2 plotter;
    curves::hermite<float, vec2f> curve;
    transform3f trans;
    camera cam;
    quad img;
    mesh_p2_points_t mesh;
    zap::engine::mesh<vertex_stream<vbuf_p2_t>, primitive_type::PT_LINE_LOOP> mesh2;
    vbuf_p2_t vbo;
    vbuf_p2_t vbo2;
    simple_shdr shdr;
};

void zap_example::initialise() {
    curve.set_points(vec2f(-1.f,-1.f), vec2f(1.f,1.f));
    curve.set_tangents(vec2f(2,0), vec2f(2,0));

    plotter.initialise();
    plotter.sample_curve(curve, 0.f, 1.f);
    gl_error_check();

    img.initialise();
    texture tex;
    tex.allocate();
    tex.initialise(10, 10, generators::planar<rgb888_t>::make_checker(10, 10, colour::red8, colour::blue8), false);
    img.set_texture(std::move(tex));
    gl_error_check();

    // Generate random points and then find convex hull
    maths::rand_lcg rnd;

    if(!mesh.allocate() || !vbo.allocate()) {
        LOG("Error Allocating mesh or vbo");
    }

    mesh.set_stream(&vbo);
    mesh.bind(); vbo.bind();

    std::vector<vec2f> hull = { vec2f(-1,-1), vec2f(0,+1), vec2f(+1,-1) };
    std::vector<vec2f> points;

    size_t sample_size = 10000;
    vbo.initialise(sample_size);
    if(vbo.map(buffer_access::BA_WRITE_ONLY)) {
        for(size_t i = 0; i != sample_size; ++i) {
            auto v = vec2f(rnd.random_s(), rnd.random_s());
            while(!is_contained(hull, v)) v = vec2f(rnd.random_s(), rnd.random_s());
            vbo[i].position = v;
            points.push_back(v);
        }
        vbo.unmap();
    }
    mesh.release();
    LOG(shdr.initialise());

    if(!mesh2.allocate() || !vbo2.allocate()) {
        LOG("Error allocating mesh2");
    }

    mesh2.set_stream(&vbo2);
    mesh2.bind(); vbo2.bind();

    auto newhull = convex_hull(points);

    LOG("Hull Size", newhull.size());

    vbo2.initialise(newhull.size(), reinterpret_cast<const char*>(newhull.data()));
    mesh2.release();

    cam.world_pos(vec3f(0,0,5));
    trans.rotate(make_rotation(float(PI)/4));
    trans.uniform_scale(0.2f);
    trans.translate(vec2f(-3.f,1.6f));
    plotter.set_mv_matrix(cam.world_to_view() * trans.gl_matrix());
    img.world_transform = trans;

    shdr.pvm_matrix(cam.proj_view());
}

void zap_example::on_resize(int width, int height) {
    application::on_resize(width, height);
    cam.viewport(0, 0, width, height);
    cam.frustum(45.f, float(width)/height, 0.5f, 100.f);
    plotter.set_projection_matrix(cam.projection());
    shdr.pvm_matrix(cam.proj_view());
}

void zap_example::on_mousemove(double x, double y) {
    constexpr static vec3f N(0,0,1);
    if(x > sc_width_ || x < 0 || y > sc_height_ || y < 0) return;
    vec3f origin, dir;

    if(cam.pick_ray(int(x), int(y), origin, dir)) {
        auto view_pos = origin - dot(origin, N)/dot(dir, N) * dir;
        auto model_pos = trans.gl_inverse().transform(view_pos);
        UNUSED(model_pos);
        //curve.set_points(model_pos.xy(), vec2f(1.f, 1.f));
        //plotter.sample_curve(curve, 0.f, 1.f);
    }
}

void zap_example::on_mousewheel(double xinc, double yinc) {
    LOG(xinc, yinc);
}

void zap_example::update(double t, float dt) {
    img.update(t, dt);
}

void zap_example::draw() {
    line_width(1.0f);
    img.draw(cam);
    plotter.draw();

    shdr.bind();
    mesh.bind();
    mesh.draw();
    gl_error_check();
    mesh.release();

    mesh2.bind();
    mesh2.draw();
    mesh2.release();
    shdr.release();
}

void zap_example::shutdown() {
}

int main(int argc, char* argv[]) {
    zap_example app;
    return app.run();
}
