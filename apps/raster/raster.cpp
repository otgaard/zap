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
    bool start_rect_;
    vec2i start_pos_;
};

bool raster::initialise() {
    img_.initialise();
    canvas_.clear_colour(vec3b(0,0,0));
    start_rect_ = false;
    return true;
}

void raster::update(double t, float dt) {
    on_resize(sc_width_,sc_height_);
}

void raster::draw() {
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

    if(!canvas_.map()) return;

    canvas_.pen_colour(colour::red8);
    canvas_.fill_colour(colour::blue8);

    int hw = (width-1)/2, hh = (height-1)/2;

    canvas_.filled_rect(100,100,200,200);
    canvas_.filled_rect(200,150,400,300);

    timer t(true);
    for(int i = 0; i < 100; ++i) {
        canvas_.pen_colour(vec3b(rand()&255,rand()&255,rand()&255));
        canvas_.fill_colour(vec3b(rand()&255, rand()&255, rand()&255));
        canvas_.ellipse(width/2, height/2, rand()%200, rand()%200);
        canvas_.filled_rect(width/2-rand()%hw, height/2-rand()%hh, width/2+rand()%hw,height/2+rand()%hh);
        canvas_.line(rand()%(width+100),rand()%(height+100),rand()%(width+100),rand()%(height+100));
    }
    LOG(t.getf(), "seconds");

    UNUSED(hh); UNUSED(hw);

    canvas_.unmap();
    canvas_.update(img_.get_texture());
}

void raster::on_mousedown(int button) {
    if(button == 0 && !start_rect_) {
        start_rect_ = true;
        start_pos_ = mouse_pos();
    } else start_rect_ = false;
}

void raster::on_mouseup(int button) {
}

void raster::on_mousemove(double x, double y) {
    application::on_mousemove(x,y);
    if(start_rect_) {
        if(!canvas_.map()) return;
        canvas_.pen_colour(colour::white8);
        canvas_.fill_colour(colour::cyan8);

        canvas_.filled_rect(start_pos_.x, start_pos_.y, mouse_pos().x, mouse_pos().y);
        canvas_.unmap();
        canvas_.update(img_.get_texture());
    }
}

void raster::on_mousewheel(double xoffset, double yoffset) {
}

int main(int argc, char* argv[]) {
    raster app;
    return app.run();
}
