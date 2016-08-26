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

    void initialise() override final;
    void update(double t, float dt) override final;
    void draw() override final;
    void shutdown() override final;

    void on_resize(int width, int height) override final;

protected:
    quad img_;
    canvas canvas_;
};

void raster::initialise() {
    img_.initialise();
    canvas_.clear(0,0,0);
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
        img_.resize(width, height);
        img_.get_texture().initialise(width, height, canvas_.buffer());
    }

    canvas_.map();
    canvas_.clear(0,0,0);

    canvas_.pen_colour(colour::white8);
    canvas_.line(0,0,width-1,height-1);

    for(int i = 0; i < 10; ++i) {
        canvas_.circle(width / 2 + 4*i, height / 2 + 4 * i, 20);
    }

    canvas_.unmap();
    canvas_.update(img_.get_texture());
}

int main(int argc, char* argv[]) {
    raster app;
    return app.run();
}
