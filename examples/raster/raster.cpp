//
// Created by Darren Otgaar on 2016/08/26.
//

#include <algorithm>
#include <maths/io.hpp>
#include <tools/log.hpp>
#include <renderer/colour.hpp>
#include "host/GLFW/application.hpp"
#include "graphics2/quad.hpp"
#include "rasteriser/canvas.hpp"

using namespace zap;
using namespace zap::maths;
using namespace zap::engine;
using namespace zap::renderer;
using namespace zap::graphics;
using namespace zap::rasteriser;

class raster : public application {
public:
    raster() : application{"raster", 600, 1024} { }
    virtual ~raster() { }

    bool initialise() override final;
    void update(double t, float dt) override final;
    void draw() override final;
    void shutdown() override final;

    void on_mousedown(int button) override final;
    void on_mouseup(int button) override final;
    void on_mousemove(double x, double y) override final;
    void on_mousewheel(double xoffset, double yoffset) override final;

    void on_resize(int width, int height) override final;

protected:
    quad img_;
    canvas canvas_;
};

bool raster::initialise() {
    img_.initialise();
    canvas_.clear_colour(vec3b(0,0,0));
    return true;
}

void raster::update(double t, float dt) {
    on_resize(sc_width_,sc_height_);
}

std::vector<vec2i> draw_pentagram(int sw, int sh, float angle) {
    std::vector<vec2f> points(5);

    float dt = float(TWO_PI<float> /5.f);
    for(int i = 0; i != 5; ++i) {
        auto theta = dt*i;
        auto ct = std::cos(theta), st = std::sin(theta);
        points[i] = vec2f(100*ct, 100*st);
    }

    std::vector<vec2f> poly(5);

    poly[0] = points[0];
    poly[1] = points[2];
    poly[2] = points[4];
    poly[3] = points[1];
    poly[4] = points[3];

    transform3f transform;
    transform.rotate(make_rotation(angle));
    transform.translate(vec2f(sw/2.f, sh/2.f));

    std::vector<vec2i> polygon;
    std::for_each(poly.begin(), poly.end(), [&polygon, &transform](const vec2f& P) {
        auto nP = transform.ptransform(P);
        polygon.push_back(vec2i(int32_t(nP.x), int32_t(nP.y)));
    });

    return polygon;
}

std::vector<vec2i> draw_shape(int sw, int sh, float angle) {
    std::vector<vec2f> poly = { {-50,-50}, {50,-50}, {-50,50}, {50,50} };

    transform3f transform;
    transform.rotate(make_rotation(angle));
    transform.translate(vec2f(sw/2.f, sh/2.f));

    std::vector<vec2i> polygon;
    std::for_each(poly.begin(), poly.end(), [&polygon, &transform](const vec2f& P) {
        auto nP = transform.ptransform(P);
        polygon.push_back(vec2i(int32_t(nP.x), int32_t(nP.y)));
    });

    return polygon;
}

std::vector<vec2i> draw_diamond(int sw, int sh, float angle) {
    std::vector<vec2f> poly = { {50,0}, {0,100}, {-50,0}, {0,-100} };

    transform3f transform;
    transform.rotate(make_rotation(angle));
    transform.translate(vec2f(sw/2.f, sh/2.f));

    std::vector<vec2i> polygon;
    std::for_each(poly.begin(), poly.end(), [&polygon, &transform](const vec2f& P) {
        auto nP = transform.ptransform(P);
        polygon.push_back(vec2i(int32_t(nP.x), int32_t(nP.y)));
    });

    return polygon;
}

std::vector<vec2i> draw_tri(int sw, int sh, float angle) {
    std::vector<vec2f> poly = { {-100,-100}, {100,-100}, {0,100} };

    transform3f transform;
    transform.rotate(make_rotation(angle));
    transform.translate(vec2f(sw/2.f, sh/2.f));

    std::vector<vec2i> polygon;
    std::for_each(poly.begin(), poly.end(), [&polygon, &transform](const vec2f& P) {
        auto nP = transform.ptransform(P);
        polygon.push_back(vec2i(int32_t(nP.x), int32_t(nP.y)));
    });

    return polygon;
}


float angle = 0.01f;

void raster::draw() {
    //const int cx = sc_width_/2, cy = sc_height_/2;

    if(!canvas_.map()) return;

    canvas_.clear();
    canvas_.fill_colour(vec3b(0,0,255));
    canvas_.pen_colour(vec3b(255,255,255));

    // Passing
    //auto polygon = draw_diamond(sc_width_, sc_height_, angle);
    //auto polygon = draw_tri(sc_width_, sc_height_, angle);
    auto polygon = draw_shape(sc_width_, sc_height_, 0);

    // Failing
    //auto polygon = draw_shape(sc_width_, sc_height_, angle);
    //auto polygon = draw_pentagram(sc_width_, sc_height_, 0);
    //LOG(angle);


    canvas_.polygon(polygon);
    canvas_.polyloop(polygon);

    angle += 0.01f;

    /*
    canvas_.fill_colour(vec3b(255,0,0));
    canvas_.filled_rect(cx-200,cy-200,cx+200,cy+200);
    canvas_.pen_colour(vec3b(255,255,255));
    canvas_.rect(cx-200,cy-200,cx+200,cy+200);
    canvas_.line(cx-200,cy-200,cx+200,cy+200);
    canvas_.line(cx-200,cy+200,cx+200,cy-200);

    canvas_.circle(cx,cy,200);

    const float inc = float(PI/30);
    for(int i = 0; i != 60; ++i) {
        canvas_.line(cx,cy,cx + std::round(200*std::cos(i*inc)), cy + std::round(200*std::sin(i*inc)));
    }

    std::vector<vec2i> polyline = { {100,100}, {200,100}, {150, 150}, {200, 50}, {75, 5} };
    canvas_.polyline(polyline);

    std::vector<vec2i> polyloop = { {480,800}, {420,900}, {380,750}, {550,890} };
    canvas_.polyloop(polyloop);

    */

    canvas_.unmap();
    canvas_.update(img_.get_texture());

    img_.draw();
}

void raster::shutdown() {
}

void raster::on_resize(int width, int height) {
    application::on_resize(width,height);

    if(canvas_.width() != width || canvas_.height() != height) {
        canvas_.resize(width, height);
        canvas_.map();
        canvas_.clear();
        img_.resize(width, height);
        canvas_.unmap();
        canvas_.update(img_.get_texture());
    }
}

void raster::on_mousedown(int button) {
}

void raster::on_mouseup(int button) {
}

void raster::on_mousemove(double x, double y) {
}

void raster::on_mousewheel(double xoffset, double yoffset) {
}

int main(int argc, char* argv[]) {
    raster app;
    return app.run();
}
