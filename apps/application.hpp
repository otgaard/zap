/* Created by Darren Otgaar on 2016/06/10. http://www.github.com/otgaard/zap */
#ifndef ZAP_APPLICATION_HPP
#define ZAP_APPLICATION_HPP

#include <string>
#include <maths/functions.hpp>

class application {
public:
    application(const std::string& name, int width, int height, bool fullscreen);
    virtual ~application() { }

    virtual bool initialise() { return true; }
    virtual void update(double t, float dt) { }
    virtual void draw() { }
    virtual void shutdown() { }

    virtual void on_keypress(char ch);
    virtual void on_resize(int width, int height);
    virtual void on_mousedown();
    virtual void on_mouseup();
    virtual void on_mousemove(double x, double y);
    virtual void on_mousewheel(double xoffset, double yoffset);

    int run();

    // TODO: Remove and replace with render_state.
    void depth_test(bool on);
    void bf_culling(bool on);
    void alpha_blending(bool on);
    void line_width(float w);

    void clear(float r=.15f, float g=.15f, float b=.15f, float a=1.f);

protected:
    int sc_width_;
    int sc_height_;
    bool fullscreen_;

private:
    std::string app_name_;
    zap::maths::timer timer_;
};

#endif //ZAP_APPLICATION_HPP
