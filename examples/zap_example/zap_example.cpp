/* Created by Darren Otgaar on 2016/03/05. http://www.github.com/otgaard/zap */
#include <map>
#include <maths/io.hpp>
#include <tools/log.hpp>
#include <host/GLFW/application.hpp>

using namespace zap;
using namespace zap::maths;

class zap_example : public application {
public:
    zap_example() : application("zap_example", 1280, 768, false) { }

    bool initialise() final;
    void update(double t, float dt) final;
    void draw() final;
    void shutdown() final;

    void on_resize(int width, int height) final;
    void on_mousemove(double x, double y) final;
    void on_mousewheel(double xinc, double yinc) final;

protected:
};

bool zap_example::initialise() {
    return true;
}

void zap_example::on_resize(int width, int height) {
}

void zap_example::on_mousemove(double x, double y) {
}

void zap_example::on_mousewheel(double xinc, double yinc) {
}

void zap_example::update(double t, float dt) {

}

void zap_example::draw() {

}

void zap_example::shutdown() {
}

int main(int argc, char* argv[]) {
    zap_example app{};
    return app.run();
}
