/* Created by Darren Otgaar on 2016/06/10. http://www.github.com/otgaard/zap */
#ifndef ZAP_APPLICATION_HPP
#define ZAP_APPLICATION_HPP

#include <string>
#include <maths/functions.hpp>

class application {
public:
    application(const std::string& name);

    virtual void initialise() { }
    virtual void update(double t, float dt) { }
    virtual void draw() { }
    virtual void shutdown() { }

    virtual void on_keypress(char ch);
    virtual void on_resize(int width, int height);

    int run();

protected:
    int sc_width_;
    int sc_height_;

private:
    std::string app_name_;
    zap::maths::timer timer_;
};

#endif //ZAP_APPLICATION_HPP
