/* Created by Darren Otgaar on 2016/08/20. http://www.github.com/otgaard/zap */
#include <renderer/colour.hpp>
#include "canvas.hpp"

using namespace zap::rasteriser;

canvas::canvas() {
    initialise();
}

canvas::canvas(int width, int height) : raster_(width, height) {
    initialise();
}

canvas::~canvas() {
}

void canvas::initialise() {
    pen_colour_ = colour::black8;
}

void canvas::resize(int width, int height) {
    raster_.resize(width, height);
}

void canvas::clear(byte r, byte g, byte b) {
    for(auto i = 0, end = raster_.size(); i != end; ++i) raster_[i].set3(r,g,b);
}

void canvas::clear(const vec3b& rgb) {
    for(auto i = 0, end = raster_.size(); i != end; ++i) raster_[i].set3(rgb);
}

void canvas::line(int x1, int y1, int x2, int y2) {
    int dx = x2 - x1, dy = y2 - y1;

    // Horizontal or Vertical Case
    if(dx == 0) {
        y1 < y2 ? vertical_line(x1, y1, y2)   : vertical_line(x1, y2, y1);
        return;
    } else if(dy == 0) {
        x1 < x2 ? horizontal_line(x1, x2, y1) : horizontal_line(x2, x1, y1);
        return;
    }

    float m = float(dy)/dx;

    if(maths::eq(maths::abs(m), 1.f)) {
        diagonal_line(x1, y1, x2, y2);
    } else if(x1 < x2) {
        if(0 < m && m < 1.f) {                          // Octant 0 - (0 .. 1)
            int d = 2*dy - dx;
            int incE = 2*dy, incNE = 2*(dy - dx);
            int x = x1, y = y1;
            raster_(x,y).set3(pen_colour_);
            while(x < x2) {
                if(d <= 0) { d += incE;  ++x; }
                else       { d += incNE; ++x; ++y; }
                raster_(x,y).set3(pen_colour_);
            }
        } else if(-1.f < m && m < 0) {                  // Octant 7 - (-1 .. 0)
            int d = 2*dy + dx;
            int incE = 2*dy, incSE = 2*(dy + dx);
            int x = x1, y = y1;
            raster_(x,y).set3(pen_colour_);
            while(x < x2) {
                if(d >= 0) { d += incE;  ++x; }
                else       { d += incSE; ++x; --y; }
                raster_(x,y).set3(pen_colour_);
            }
        } else if(1.f < m) {                            // Octant 1 - (1 .. inf)
            std::swap(dx, dy);
            int d = 2*dy - dx;
            int incN = 2*dy, incNE = 2*(dy - dx);
            int x = x1, y = y1;
            raster_(x,y).set3(pen_colour_);
            while(y < y2) {
                if(d <= 0) { d += incN;  ++y; }
                else       { d += incNE; ++y; ++x; }
                raster_(x,y).set3(pen_colour_);
            }
        } else if(m < -1.f) {                           // Octant 6 - (-inf .. -1)
            std::swap(dx, dy);
            int d = 2*dy + dx;
            int incS = 2*dy, incSE = 2*(dy + dx);
            int x = x1, y = y1;
            raster_(x,y).set3(pen_colour_);
            while(y > y2) {
                if(d <= 0) { d += incS;  --y; }
                else       { d += incSE; --y; ++x; }
                raster_(x,y).set3(pen_colour_);
            }
        }
    } else { // x2 < x1
        if(0 < m && m < 1.f) {                          // Octant 4 - (0 .. 1)
            int d = 2*dy - dx;
            int incW = 2*dy, incSW = 2*(dy - dx);
            int x = x1, y = y1;
            raster_(x,y).set3(pen_colour_);
            while(x > x2) {
                if(d > 0) { d += incW;  --x; }
                else      { d += incSW; --x; --y; }
                raster_(x,y).set3(pen_colour_);
            }
        } else if(-1.f < m && m < 0) {                  // Octant 3 - (-1 .. 0)
            int d = 2*dy + dx;
            int incW = 2*dy, incNW = 2*(dy + dx);
            int x = x1, y = y1;
            raster_(x,y).set3(pen_colour_);
            while(x > x2) {
                if(d < 0) { d += incW;  --x; }
                else      { d += incNW; --x; ++y; }
                raster_(x,y).set3(pen_colour_);
            }
        } else if(1.f < m) {                            // Octant 5 - (1 .. inf)
            std::swap(dx, dy);
            int d = 2*dy - dx;
            int incS = 2*dy, incSW = 2*(dy - dx);
            int x = x1, y = y1;
            raster_(x,y).set3(pen_colour_);
            while(y > y2) {
                if(d > 0) { d += incS;  --y; }
                else      { d += incSW; --y; --x; }
                raster_(x,y).set3(pen_colour_);
            }
        } else if(m < -1.f) {                           // Octant 2 - (-inf .. -1)
            std::swap(dx, dy);
            int d = 2*dy + dx;
            int incN = 2*dy, incNW = 2*(dy + dx);
            int x = x1, y = y1;
            raster_(x,y).set3(pen_colour_);
            while(y < y2) {
                if(d > 0) { d += incN;  ++y; }
                else      { d += incNW; ++y; --x; }
                raster_(x,y).set3(pen_colour_);
            }
        }
    }
}

void canvas::vertical_line(int x1, int y1, int y2) {
    assert(y1 < y2 && "vertical_line requires y1 < y2");
    int y = y1;
    while(y <= y2) raster_(x1,y++).set3(pen_colour_);
}

void canvas::horizontal_line(int x1, int x2, int y1) {
    assert(x1 < x2 && "horizontal_line requires x1 < x2");
    int x = x1;
    while(x <= x2) raster_(x++,y1).set3(pen_colour_);
}

void canvas::diagonal_line(int x1, int y1, int x2, int y2) {
    int xd = maths::sign(x2 - x1), yd = maths::sign(y2 - y1);
    int x = x1, y = y1;
    if(xd > 0) {
        while(x <= x2) {
            raster_(x, y).set3(pen_colour_);
            x += xd;
            y += yd;
        }
    } else {
        while(x >= x2) {
            raster_(x, y).set3(pen_colour_);
            x += xd;
            y += yd;
        }
    }
}

/*
 * Old Line Implementation Comparisons
 */

/*
// Very inefficient
void canvas::line1(int x1, int y1, int x2, int y2) {
    const float m = float(y2 - y1)/(x2 - x1);
    const float C = y1;

    float curr_y = C;
    for(int c = x1; c <= x2; ++c) {
        curr_y = m*c + C;
        int y = int(.5f+curr_y);
        raster_(c, y).set3(pen_colour_);
    }
}

// This will drift
void canvas::line2(int x1, int y1, int x2, int y2) {
    const float m = float(y2 - y1)/(x2 - x1);
    float curr_y = y1;
    for(int c = x1; c <= x2; ++c) {
        curr_y += m;
        int y = int(.5f+curr_y);
        raster_(c, y).set3(pen_colour_);
    }
}

// Too slow
void canvas::line4(int x1, int y1, int x2, int y2) {
    int dx = x2 - x1, dy = y2 - y1;
    if(dx || dy) {
        if(abs(dx) >= abs(dy)) {
            float y = y1+.5f; float dly = float(dy)/dx;
            if(dx > 0) {
                for(int x = x1; x <= x2; ++x) {
                    raster_(x,int(floor(y))).set3(pen_colour_);
                    y += dly;
                }
            } else {
                for(int x = x1; x >= int(x2); --x) {
                    raster_(x,int(floor(y))).set3(pen_colour_);
                    y -= dly;
                }
            }
        } else {
            float x = x1+.5f;
            float dlx = float(dx)/dy;
            if(dy > 0) {
                for(int y = y1; y <= y2; ++y) {
                    raster_(int(floor(x)),y).set3(pen_colour_);
                    x += dlx;
                }
            } else {
                for(int y = y1; y >= int(y2); --y) {
                    raster_(int(floor(x)), y).set3(pen_colour_);
                    x -= dlx;
                }
            }
        }
    }
}
*/