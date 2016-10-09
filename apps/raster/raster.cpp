//
// Created by Darren Otgaar on 2016/08/26.
//

#define LOGGING_ENABLED
#include <maths/io.hpp>
#include <tools/log.hpp>
#include <renderer/colour.hpp>
#include "apps/application.hpp"
#include "graphics2/quad.hpp"
#include "rasteriser/canvas.hpp"

using namespace zap;
using namespace zap::maths;
using namespace zap::engine;
using namespace zap::renderer;
using namespace zap::graphics2;
using namespace zap::rasteriser;

class raster : public application {
public:
    raster() : application("raster", 600, 1024, false) { }
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

float angle = 0.f;

void raster::draw() {
    //const int cx = sc_width_/2, cy = sc_height_/2;

    if(!canvas_.map()) return;

    canvas_.clear();
    canvas_.fill_colour(vec3b(0,0,255));
    canvas_.pen_colour(vec3b(255,255,255));

    static std::vector<vec2f> poly = { {300.f,612.f}, {350.f,700.f}, {320.f,800.f} };

    auto centre = std::accumulate(poly.begin(), poly.end(), vec2f(0,0));
    centre *= 1/3.f;
    LOG(centre);

    transform3f transform;
    transform.rotate(make_rotation(angle));
    transform.translate(centre);

    std::vector<vec2i> polygon;
    std::for_each(poly.begin(), poly.end(), [&polygon, &transform, &centre](const vec2f& P) {
        auto nP = transform.ptransform(P-centre);
        LOG(nP);
        polygon.push_back(vec2i(nP.x, nP.y));
    });

    // Rotate the polygon so we can check the fill algorithm


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
