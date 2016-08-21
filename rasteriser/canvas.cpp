/* Created by Darren Otgaar on 2016/08/20. http://www.github.com/otgaard/zap */
#include <renderer/colour.hpp>
#include "canvas.hpp"

using namespace zap::engine;
using namespace zap::rasteriser;

canvas::canvas() : is_mapped_(false) {
}

canvas::canvas(int width, int height) : is_mapped_(false) {
    raster_.resize(width, height);
    initialise();
}

canvas::~canvas() {
}

void canvas::map() {
    if(is_mapped_) return;
    min_.x = width(); min_.y = height();
    max_.x = 0;       max_.y = 0;
    is_mapped_ = true;
}

void canvas::unmap() {
    is_mapped_ = false;
}

void canvas::initialise() {
    pen_colour_ = colour::black8;
    cmin_.set(0,0); cmax_.set(width(),height());
}

void canvas::update(texture& tex) {
    if(is_mapped_) { LOG("Error, unmap canvas to update texture"); return; }

    max_.x += 1; max_.y += 1;
    auto w = max_.x - min_.x, h = max_.y - min_.y;
    if(w > 0 && h > 0) {
        std::vector<rgb888_t> subwindow(w * h);
        for(int r = 0; r != h; ++r) {
            std::copy(raster_.data(raster_.offset(min_.x, min_.y+r)), raster_.data(raster_.offset(max_.x, min_.y+r)),
                      subwindow.begin()+(r*w));
        }

        if(!pbo_.is_allocated()) pbo_.allocate();
        pbo_.bind(true);
        pbo_.initialise(w, h, subwindow.data());
        tex.copy(pbo_, min_.x, min_.y, w, h, 0);
    }
}

inline void canvas::update_region(int x, int y) {
    if(x < min_.x)  min_.x = x;
    if(x < cmin_.x) cmin_.x = x;
    if(x > max_.x)  max_.x = x;
    if(x > cmax_.x) cmax_.x = x;
    if(y < min_.y)  min_.y = y;
    if(y < cmin_.y) cmin_.y = y;
    if(y > max_.y)  max_.y = y;
    if(y > cmax_.y) cmax_.y = y;
}

void canvas::resize(int width, int height) {
    raster_.resize(width, height);
    cmin_.set(0,0); cmax_.set(width-1,height-1);
    clear();
}

void canvas::clear(byte r, byte g, byte b) {
    for(int y = cmin_.y, yend = std::min(cmax_.y,height()-1); y <= yend; ++y) {
        for(int x = cmin_.x, xend = std::min(cmax_.x,width()-1); x <= xend; ++x) {
            raster_[raster_.offset(x,y)].set3(r,g,b);
        }
    }
    min_ = cmin_; max_ = cmax_;
    cmin_.set(width(), height()); cmax_.set(0,0);
}

void canvas::clear(const vec3b& rgb) {
    for(int y = cmin_.y, yend = std::min(cmax_.y,height()-1); y <= yend; ++y) {
        for(int x = cmin_.x, xend = std::min(cmax_.x,width()-1); x <= xend; ++x) {
            raster_[raster_.offset(x,y)].set3(rgb);
        }
    }
    min_ = cmin_; max_ = cmax_;
    cmin_.set(width(), height()); cmax_.set(0,0);
}

void canvas::line(int x1, int y1, int x2, int y2) {
    update_region(x1,y1); update_region(x2,y2);

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