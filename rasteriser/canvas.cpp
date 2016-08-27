/* Created by Darren Otgaar on 2016/08/20. http://www.github.com/otgaard/zap */
#include <renderer/colour.hpp>
#include "canvas.hpp"

using namespace zap::engine;
using namespace zap::rasteriser;

canvas::canvas() : mapped_ptr_(nullptr) /*is_mapped_(false)*/ {
}

canvas::canvas(int width, int height) : mapped_ptr_(nullptr) /*is_mapped_(false)*/ {
    //raster_.resize(width, height);
    initialise();
}

canvas::~canvas() {
}

bool canvas::map() {
    if(mapped_ptr_) return true;

    //if(is_mapped_) return;
    min_.x = width(); min_.y = height();
    max_.x = 0;       max_.y = 0;
    //is_mapped_ = true;
    raster_.bind();
    mapped_ptr_ = reinterpret_cast<rgb888_t*>(raster_.map(buffer_access::BA_WRITE_ONLY, true));
    //LOG("mapped", mapped_ptr_);
    return mapped_ptr_ != nullptr;
}

void canvas::unmap() {
    raster_.unmap(true);
    raster_.release();
    //is_mapped_ = false;
    mapped_ptr_ = nullptr;
}

void canvas::initialise() {
    pen_colour_ = colour::black8;
}

void canvas::update(texture& tex) {
    if(mapped_ptr_) { LOG("Error, unmap canvas to update texture"); return; }

    max_.x += 1; max_.y += 1;
    auto w = max_.x - min_.x, h = max_.y - min_.y;
    LOG(min_.x, min_.y, w, h);
    if(w > 0 && h > 0) {
        if(tex.width() != width() || tex.height() != height()) {
            tex.initialise(texture_type::TT_TEX2D,width(),height(),pixel_format::PF_RGB, pixel_datatype::PD_UNSIGNED_BYTE);
        }

        if(w == width() && h == height()) {
            LOG("A");
            raster_.bind();
            tex.copy(raster_, 0, 0, w, h, 0);
            raster_.release();
        } else if(w > width()-int(0.1f*width())) { // Perhaps calculate some percentage width of the scan line over which we just copy the block
            LOG("B");
            if(!pbo_.is_allocated()) pbo_.allocate();
            pbo_.bind();
            pbo_.initialise(width(), h);
            raster_.bind(buffer_type::BT_COPY_READ);
            pbo_.copy_buffer(buffer_type::BT_COPY_READ, buffer_type::BT_PIXEL_UNPACK,
                             raster_.idx(0, min_.y) * raster_.pixel_size(), 0,
                             h * width() * raster_.pixel_size());
            raster_.release(buffer_type::BT_COPY_READ);
            pbo_.bind();
            tex.copy(pbo_, 0, min_.y, width(), h, 0);
            pbo_.release();
        } else {
            LOG("C");
            if(!pbo_.is_allocated()) pbo_.allocate();
            pbo_.bind();
            pbo_.initialise(w, h);
            raster_.bind(buffer_type::BT_COPY_READ);
            for(int r = 0; r != h; ++r) {
                pbo_.copy_buffer(buffer_type::BT_COPY_READ, buffer_type::BT_PIXEL_UNPACK,
                                 raster_.idx(min_.x, min_.y + r) * raster_.pixel_size(), r * w * raster_.pixel_size(),
                                 w * raster_.pixel_size());
            }
            raster_.release(buffer_type::BT_COPY_READ);
            pbo_.bind();
            tex.copy(pbo_, min_.x, min_.y, w, h, 0);
            pbo_.release();
        }
    }
}

inline void canvas::update_region(int x, int y) {
    if(x < min_.x)  min_.x = x;
    if(x > max_.x)  max_.x = x;
    if(y < min_.y)  min_.y = y;
    if(y > max_.y)  max_.y = y;
}

void canvas::resize(int width, int height) {
    if(raster_.is_mapped()) { LOG_ERR("Cannot resize canvas while raster is mapped"); return; }

    if(!raster_.is_allocated()) {
        //LOG("Allocated", width, height);
        raster_.allocate();
        raster_.bind();
        raster_.initialise(width, height);
        raster_.release();
    } else {
        raster_.bind();
        raster_.resize(width, height);
        if(map()) clear();
        unmap();
    }
}

void canvas::clear(byte r, byte g, byte b) {
    for(int y = 0, yend = height()-1; y <= yend; ++y) {
        for(int x = 0, xend = width()-1; x <= xend; ++x) {
            raster_(x,y).set3(r,g,b);
        }
    }
    min_.set(0,0), max_.set(width()-1,height()-1);
}

void canvas::clear(const vec3b& rgb) {
    for(int y = 0, yend = height()-1; y <= yend; ++y) {
        for(int x = 0, xend = width()-1; x <= xend; ++x) {
            raster_(x,y).set3(rgb);
        }
    }
    min_.set(0,0), max_.set(width()-1,height()-1);
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

void canvas::circle(int cx, int cy, int r) {
    update_region(cx-r,cy-r); update_region(cx+r,cy+r);

    int x = 0, y = r, d = 1 - r;
    int incE = 3, incSE = -2 * r + 5;
    circle_points(cx, cy, x, y);

    while(y > x) {
        if(d < 0) {
            d += incE; incE += 2; incSE += 2;
        } else {
            d += incSE; incE += 2; incSE += 4; y--;
        }
        ++x;
        circle_points(cx, cy, x, y);
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

void canvas::circle_points(int cx, int cy, int x, int y) {
    raster_(cx + x, cy + y).set3(pen_colour_); raster_(cx + x, cy - y).set3(pen_colour_);
    raster_(cx - x, cy + y).set3(pen_colour_); raster_(cx - x, cy - y).set3(pen_colour_);
    raster_(cx + y, cy + x).set3(pen_colour_); raster_(cx + y, cy - x).set3(pen_colour_);
    raster_(cx - y, cy + x).set3(pen_colour_); raster_(cx - y, cy - x).set3(pen_colour_);
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