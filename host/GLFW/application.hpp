/* Created by Darren Otgaar on 2016/06/10. http://www.github.com/otgaard/zap */
#ifndef ZAP_APPLICATION_HPP
#define ZAP_APPLICATION_HPP

#include <string>
#include <maths/functions.hpp>
#include <maths/vec2.hpp>

struct GLFWwindow;

class application {
public:
    application(const std::string& name, int width, int height, bool fullscreen, bool resizable=true);
    virtual ~application() = default;

    virtual bool initialise() { return true; }
    virtual void update(double t, float dt) { }
    virtual void draw() { }
    virtual void shutdown() { }

    virtual void on_keypress(char ch);
    virtual void on_keyrelease(char ch);
    virtual void on_resize(int width, int height);
    virtual void on_mousedown(int button);
    virtual void on_mouseup(int button);
    virtual void on_mousemove(double x, double y);
    virtual void on_mousewheel(double xoffset, double yoffset);

    int run();

    zap::maths::vec2i mouse_pos() const { return mouse_; }
    void set_dims(int width, int height) { sc_width_ = width; sc_height_ = height; }
    int width() const { return sc_width_; }
    int height() const { return sc_height_; }
    void resize(int width, int height);

protected:
    int sc_width_;
    int sc_height_;
    bool fullscreen_;
    bool resizeable_;
    zap::maths::vec2i mouse_;

private:
    std::string app_name_;
    zap::maths::timer timer_;
    GLFWwindow* window_ = nullptr;
};

#endif //ZAP_APPLICATION_HPP
