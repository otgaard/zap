/* Created by Darren Otgaar on 2016/08/20. http://www.github.com/otgaard/zap */

#include <renderer/colour.hpp>
#include "canvas.hpp"

#include <algorithm>
#include <maths/io.hpp>

using namespace zap::engine;
using namespace zap::rasteriser;

template <typename PixelT>
canvas<PixelT, pixel_buffer>::canvas() : mapped_ptr_(nullptr) {
}

template <typename PixelT>
canvas<PixelT, pixel_buffer>::canvas(int width, int height) : mapped_ptr_(nullptr), clip_region_(0, width-1, 0, height-1) {
    initialise();
}

template <typename PixelT>
canvas<PixelT, pixel_buffer>::~canvas() {
}

template <typename PixelT>
bool canvas<PixelT, pixel_buffer>::map() {
    if(mapped_ptr_) return true;

    min_.x = width(); min_.y = height();
    max_.x = 0;       max_.y = 0;
    raster_.bind();
    mapped_ptr_ = reinterpret_cast<pixel_t*>(raster_.map(buffer_access::BA_WRITE_ONLY, true));
    return mapped_ptr_ != nullptr;
}

template <typename PixelT>
void canvas<PixelT, pixel_buffer>::unmap() {
    raster_.unmap(true);
    raster_.release();
    mapped_ptr_ = nullptr;
    min_.x = maths::clamp(min_.x, 0, width()); max_.x = maths::clamp(max_.x, 0, width());
    min_.y = maths::clamp(min_.y, 0, height()); max_.y = maths::clamp(max_.y, 0, height());
}

template <typename PixelT>
void canvas<PixelT, pixel_buffer>::initialise() {
    pen_colour_ = colour::black8;
    fill_colour_ = colour::white8;
    clear_colour_ = colour::white8;
}

template <typename PixelT>
void canvas<PixelT, pixel_buffer>::update(texture& tex) {
    if(mapped_ptr_) { LOG("Error, unmap canvas to update texture"); return; }

    max_.x += 1; max_.y += 1;
    auto w = max_.x - min_.x, h = max_.y - min_.y;
    if(w > 0 && h > 0) {
        if(tex.width() != width() || tex.height() != height()) {
            tex.initialise(texture_type::TT_TEX2D,width(),height(),1,pixel_format::PF_RGB, pixel_datatype::PD_UNSIGNED_BYTE,false);
        }

        if(w == width() && h == height()) {
            raster_.bind();
            tex.copy(raster_, 0, 0, w, h, 0);
            raster_.release();
        } else if(w > block_width_) { // Perhaps calculate some percentage width of the scan line over which we just copy the block
            if(!pbo_.is_allocated()) pbo_.allocate();
            pbo_.bind();
            pbo_.initialise(width(), h);
            raster_.bind(buffer_type::BT_COPY_READ);
            pbo_.copy_buffer(buffer_type::BT_COPY_READ, buffer_type::BT_PIXEL_UNPACK,
                             raster_.idx(0, min_.y) * raster_.pixel_size(),
                             0,
                             h * width() * raster_.pixel_size());
            raster_.release(buffer_type::BT_COPY_READ);
            pbo_.bind();
            tex.copy(pbo_, 0, min_.y, width(), h, 0);
            pbo_.release();
        } else {
            if(!pbo_.is_allocated()) pbo_.allocate();
            pbo_.bind();
            pbo_.initialise(w, h);
            raster_.bind(buffer_type::BT_COPY_READ);
            for(int r = 0; r != h; ++r) {
                pbo_.copy_buffer(buffer_type::BT_COPY_READ, buffer_type::BT_PIXEL_UNPACK,
                                 raster_.idx(min_.x, min_.y + r) * raster_.pixel_size(),
                                 r * w * raster_.pixel_size(),
                                 w * raster_.pixel_size());
            }
            raster_.release(buffer_type::BT_COPY_READ);
            pbo_.bind();
            tex.copy(pbo_, min_.x, min_.y, w, h, 0);
            pbo_.release();
        }
    }
}

template <typename PixelT>
inline void canvas<PixelT, pixel_buffer>::update_region(int x, int y) {
    if(x < min_.x)  min_.x = x;
    if(x > max_.x)  max_.x = x;
    if(y < min_.y)  min_.y = y;
    if(y > max_.y)  max_.y = y;
}

template <typename PixelT>
void canvas<PixelT, pixel_buffer>::resize(int width, int height) {
    if(raster_.is_mapped()) { LOG_ERR("Cannot resize canvas while raster is mapped"); return; }

    if(!raster_.is_allocated()) {
        raster_.allocate();
        raster_.bind();
        raster_.initialise(width, height);
        raster_.release();
    } else {
        raster_.bind();
        raster_.resize(width, height);
        if(map()) {
            clear(clear_colour_);
            unmap();
        }
    }

    block_width_ = int(.8f*width);      // Update width over which we copy an entire block of memory
    clip_region_.set(0, width-1, 0, height-1);
}

template <typename PixelT>
void canvas<PixelT, pixel_buffer>::clear(byte r, byte g, byte b) {
    for(int y = 0, yend = height()-1; y <= yend; ++y) {
        for(int x = 0, xend = width()-1; x <= xend; ++x) {
            raster_(x,y).set3(r,g,b);
        }
    }
    min_.set(0,0), max_.set(width()-1,height()-1);
}

template <typename PixelT>
void canvas<PixelT, pixel_buffer>::clear(const vec3b& rgb) {
    for(int y = 0, yend = height()-1; y <= yend; ++y) {
        for(int x = 0, xend = width()-1; x <= xend; ++x) {
            raster_(x,y).set3(rgb);
        }
    }
    min_.set(0,0), max_.set(width()-1,height()-1);
}

template <typename PixelT>
void canvas<PixelT, pixel_buffer>::clear() {
    for(int y = 0, yend = height()-1; y <= yend; ++y) {
        for(int x = 0, xend = width()-1; x <= xend; ++x) {
            raster_(x,y).set3(clear_colour_);
        }
    }
    min_.set(0,0), max_.set(width()-1,height()-1);
}

template <typename PixelT>
void canvas<PixelT, pixel_buffer>::line_impl(int x1, int y1, int x2, int y2) {
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

template <typename PixelT>
void canvas<PixelT, pixel_buffer>::circle(int cx, int cy, int r) {
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

template <typename PixelT>
void canvas<PixelT, pixel_buffer>::ellipse(int cx, int cy, int major, int minor) {
    update_region(cx-major,cy-minor); update_region(cx+major,cy+minor);

    const int major2 = 2*major*major, minor2 = 2*minor*minor;
    int x = -major, y = 0;
    int e2 = minor, dx = (1+2*x)*e2*e2;
    int dy = x*x, err = dx+dy;
    do {
        ellipse_points(cx, cy, x, y);
        e2 = 2*err;
        if(e2 >= dx) { x++; dx += minor2; err += dx; }
        if(e2 <= dy) { y++; dy += major2; err += dy; }
    } while(x <= 0);

    while(y++ < minor) ellipse_points(cx, cy, x, y);
}

template <typename PixelT>
void canvas<PixelT, pixel_buffer>::rect(int x1, int y1, int x2, int y2) {
    update_region(x1,y1); update_region(x2,y2);
    int left, bottom, right, top;
    if(x1 < x2) { left = x1; right = x2; }
    else        { left = x2; right = x1; }
    if(y1 < y2) { bottom = y1; top = y2; }
    else        { bottom = y2; top = y1; }

    // Use principle of bottom and left being part of primitive, top and right, not.
    line_impl(left, bottom, right, bottom);
    line_impl(right, bottom, right, top);
    line_impl(left, top, right, top);
    line_impl(left, bottom, left, top);
}

template <typename PixelT>
void canvas<PixelT, pixel_buffer>::filled_rect(int x1, int y1, int x2, int y2) {
    update_region(x1,y1); update_region(x2,y2);
    int left, bottom, right, top;
    if(x1 < x2) { left = x1; right = x2; }
    else        { left = x2; right = x1; }
    if(y1 < y2) { bottom = y1; top = y2; }
    else        { bottom = y2; top = y1; }

    if(right-left <= 0) return;

    // Use principle of bottom and left being part of primitive, top and right, not.
    for(int c = left; c < right; ++c) raster_(c,bottom).set3(fill_colour_);

    const auto diff = right - left;
    for(int r = bottom+1; r < top; ++r) {
        raster_.copy(raster_.idx(left,bottom), raster_.idx(left,r), diff);
    }
}

template <typename PixelT>
void canvas<PixelT, pixel_buffer>::vertical_line(int x1, int y1, int y2) {
    if(y1 < y2) std::swap(y1, y2); //assert(y1 < y2 && "vertical_line requires y1 < y2");
    int y = y1;
    while(y <= y2) raster_(x1,y++).set3(pen_colour_);
}

template <typename PixelT>
void canvas<PixelT, pixel_buffer>::horizontal_line(int x1, int x2, int y1) {
    assert(x1 < x2 && "horizontal_line requires x1 < x2");
    int x = x1;
    while(x <= x2) raster_(x++,y1).set3(pen_colour_);
}

template <typename PixelT>
void canvas<PixelT, pixel_buffer>::diagonal_line(int x1, int y1, int x2, int y2) {
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

template <typename PixelT>
void canvas<PixelT, pixel_buffer>::circle_points(int cx, int cy, int x, int y) {
    raster_(cx + x, cy + y).set3(pen_colour_); raster_(cx + x, cy - y).set3(pen_colour_);
    raster_(cx - x, cy + y).set3(pen_colour_); raster_(cx - x, cy - y).set3(pen_colour_);
    raster_(cx + y, cy + x).set3(pen_colour_); raster_(cx + y, cy - x).set3(pen_colour_);
    raster_(cx - y, cy + x).set3(pen_colour_); raster_(cx - y, cy - x).set3(pen_colour_);
}

template <typename PixelT>
void canvas<PixelT, pixel_buffer>::ellipse_points(int cx, int cy, int x, int y) {
    raster_(cx + x, cy + y).set3(pen_colour_); raster_(cx - x, cy + y).set3(pen_colour_);
    raster_(cx + x, cy - y).set3(pen_colour_); raster_(cx - x, cy - y).set3(pen_colour_);
}

enum clip_plane {
    CP_TOP    = 1 << 0,
    CP_BOTTOM = 1 << 1,
    CP_RIGHT  = 1 << 2,
    CP_LEFT   = 1 << 3
};

// Cohen-Sutherland line clipper
template <typename PixelT>
void canvas<PixelT, pixel_buffer>::line(int x1, int y1, int x2, int y2) {
    auto region = [](int x, int y, int xmin, int xmax, int ymin, int ymax) {
        int code = 0;
        if(y > ymax)      code |= clip_plane::CP_TOP;
        else if(y < ymin) code |= clip_plane::CP_BOTTOM;
        if(x > xmax)      code |= clip_plane::CP_RIGHT;
        else if(x < xmin) code |= clip_plane::CP_LEFT;
        return code;
    };

    int code1, code2, codeOut;
    bool accept = false, done = false;
    code1 = region(x1, y1, clip_region_.left, clip_region_.right, clip_region_.bottom, clip_region_.top);
    code2 = region(x2, y2, clip_region_.left, clip_region_.right, clip_region_.bottom, clip_region_.top);
    do {
        if(!(code1 | code2)) { accept = true; done = true; }        // Whole line contained
        else if(code1 & code2) done = true;                         // Whole line rejected
        else {
            int x=0, y=0;
            codeOut = code1 ? code1 : code2;
            if(codeOut & CP_TOP) {
                x = x1 + (x2 - x1) * (clip_region_.top - y1) / (y2 - y1);
                y = clip_region_.top;
            } else if(codeOut & CP_BOTTOM) {
                x = x1 + (x2 - x1) * (clip_region_.bottom - y1) / (y2 - y1);
                y = clip_region_.bottom;
            } else if(codeOut & CP_RIGHT) {
                y = y1 + (y2 - y1) * (clip_region_.right - x1) / (x2 - x1);
                x = clip_region_.right;
            } else if(codeOut & CP_LEFT) {
                y = y1 + (y2 - y1) * (clip_region_.left - x1) / (x2 - x1);
                x = clip_region_.left;
            }

            if(codeOut == code1) {
                x1 = x; y1 = y;
                code1 = region(x1, y1, clip_region_.left, clip_region_.right, clip_region_.bottom, clip_region_.top);
            } else {
                x2 = x; y2 = y;
                code2 = region(x2, y2, clip_region_.left, clip_region_.right, clip_region_.bottom, clip_region_.top);
            }
        }
    } while(!done);

    if(accept) line_impl(x1, y1, x2, y2);
}

template <typename PixelT>
void canvas<PixelT, pixel_buffer>::polyline(const std::vector<segment2i>& polyline) {
    for(const auto& l : polyline) {
        line(l.P0, l.P1);
    }
}

template <typename PixelT>
void canvas<PixelT, pixel_buffer>::polyline(const std::vector<vec2i>& polyline) {
    auto size = polyline.size();
    if(size < 2) return;
    for(size_t i = 0, j = 1; j != size; i = j, ++j) line(polyline[i], polyline[j]);
}

template <typename PixelT>
void canvas<PixelT, pixel_buffer>::polyloop(const std::vector<vec2i>& polyloop) {
    auto size = polyloop.size();
    if(size < 2) return;
    for(size_t i = 0, j = 1; j != size; i = j, ++j) line(polyloop[i], polyloop[j]);
    size--;
    line(polyloop[size], polyloop[0]);
}

// Algorithm based on Computer Graphics - Principles and Practice (2nd) [91 - 99]

struct edge_table {
    int min_y, max_y;

    struct edge {
        int ymax;
        int xmin;
        int sign;
        int numerator;
        int denominator;
        int increment;
    };

    struct bucket {
        int y;
        std::vector<edge> edges;
    };

    std::vector<bucket> buckets;

    edge_table(const std::vector<vec2i>& polygon) {
        auto vl = polygon;
        std::tie(min_y, max_y) = zap::maths::find_range(polygon, [](const vec2i& v) { return v.y; });

        std::sort(vl.begin(), vl.end(), [](const vec2i& A, const vec2i& B) {
            return (A.y < B.y) || (A.y == B.y && A.x < B.x);
        });

        size_t counter = 0;
        while(counter != vl.size()) {
            auto it = std::find(polygon.begin(), polygon.end(), vl[counter]);
            if(it->y == max_y) break; // We must be at the last vertex or one of the last vertices

            auto i = it - polygon.begin();
            auto left = i == 0 ? int(polygon.size())-1 : i-1, right = i == int(polygon.size())-1 ? 0 : i+1;
            if(polygon[left].x > polygon[right].x) std::swap(left,right);

            if(polygon[left].y < it->y && polygon[right].y < it->y) { counter++; continue; }
            bucket nb;
            nb.y = it->y;
            if(polygon[left].y > nb.y) {
                auto& l = polygon[left];
                edge e1;
                e1.ymax = l.y;
                e1.numerator = l.x - it->x;
                e1.denominator = l.y - it->y;
                e1.sign = zap::maths::sign(e1.numerator);
                e1.numerator = std::abs(e1.numerator);  // Make the numerator positive, the sign stores inc/dec
                e1.increment = e1.denominator;          // Denominator is always positive
                e1.xmin = it->x;
                nb.edges.emplace_back(std::move(e1));
            }

            if(polygon[right].y > nb.y) {
                auto& r = polygon[right];
                edge e2;
                e2.ymax = r.y;
                e2.numerator = r.x - it->x;
                e2.denominator = r.y - it->y;
                e2.sign = zap::maths::sign(e2.numerator);
                e2.numerator = std::abs(e2.numerator);  // Make the numerator positive, the sign stores inc/dec
                e2.increment = e2.denominator;          // Denominator is always positive
                e2.xmin = it->x;
                nb.edges.emplace_back(std::move(e2));
            }
            if(!nb.edges.empty()) buckets.emplace_back(std::move(nb));
            ++counter;
        }
    }
};

template <typename PixelT>
void canvas<PixelT, pixel_buffer>::polygon(const std::vector<vec2i>& polygon) {
    if(polygon.size() < 3) { LOG_ERR("Polygon must consist of at least three vertices"); return; }
    edge_table global_et(polygon);
    if(global_et.buckets.size() == 0) { LOG_ERR("Edge Table empty"); return; }

    std::vector<edge_table::edge> AET;

    int curr_bucket = 0;
    int curr_y = global_et.min_y;
    while(curr_y != global_et.max_y) {
        while(global_et.buckets[curr_bucket].y == curr_y) {
            auto& current = global_et.buckets[curr_bucket];
            std::copy(current.edges.begin(), current.edges.end(), std::back_inserter(AET));
            ++curr_bucket;
        }

        auto it = std::remove_if(std::begin(AET), std::end(AET), [&curr_y](const edge_table::edge& ed) {
            return ed.ymax == curr_y;
        });
        if(it != std::end(AET)) AET.erase(it, AET.end());
        if(AET.size() % 2 != 0) { LOG_ERR("AET contains non-even number of edges"); return; }

        std::sort(std::begin(AET), std::end(AET), [](const edge_table::edge& A, const edge_table::edge& B) {
            return A.xmin < B.xmin;
        });

        for(int i = 0, end = (int)AET.size(); i != end; i += 2) {
            auto& se = AET[2*i]; auto& ee = AET[2*i+1];
            for(int x = se.xmin, xend = ee.xmin; x <= xend; ++x) {
                raster_(x,curr_y).set3(fill_colour_);
            }
            if(se.denominator != 0) {
                se.increment += se.numerator;
                while(se.increment > se.denominator) {
                    se.xmin += se.sign; se.increment -= se.denominator;
                }
            }
            if(ee.denominator != 0) {
                ee.increment += ee.numerator;
                while(ee.increment > ee.denominator) {
                    ee.xmin += ee.sign; ee.increment -= ee.denominator;
                }
            }
        }

        ++curr_y;
    }
}

template <typename PixelT>
canvas<PixelT, pixmap>::canvas() : mapped_ptr_(nullptr) {
}

template <typename PixelT>
canvas<PixelT, pixmap>::canvas(int width, int height) : mapped_ptr_(nullptr), clip_region_(0, width-1, 0, height-1) {
    initialise();
}

template <typename PixelT>
canvas<PixelT, pixmap>::~canvas() {
}

template <typename PixelT>
void canvas<PixelT, pixmap>::initialise() {
    pen_colour_ = colour::black8;
    fill_colour_ = colour::white8;
    clear_colour_ = colour::white8;
}

template <typename PixelT>
void canvas<PixelT, pixmap>::update(texture& tex) {
    if(mapped_ptr_) { LOG("Error, unmap canvas to update texture"); return; }

    max_.x += 1; max_.y += 1;
    auto w = max_.x - min_.x, h = max_.y - min_.y;
    if(w > 0 && h > 0) {
        if(tex.width() != width() || tex.height() != height()) {
            tex.initialise(raster_);
        }
    }
}

template <typename PixelT>
inline void canvas<PixelT, pixmap>::update_region(int x, int y) {
    if(x < min_.x)  min_.x = x;
    if(x > max_.x)  max_.x = x;
    if(y < min_.y)  min_.y = y;
    if(y > max_.y)  max_.y = y;
}

template <typename PixelT>
void canvas<PixelT, pixmap>::resize(int width, int height) {
    raster_.resize(width, height);
    clear(clear_colour_);
    clip_region_.set(0, width-1, 0, height-1);
}

template <typename PixelT>
void canvas<PixelT, pixmap>::clear(byte r, byte g, byte b) {
    for(int y = 0, yend = height()-1; y <= yend; ++y) {
        for(int x = 0, xend = width()-1; x <= xend; ++x) {
            raster_(x,y).set3(r,g,b);
        }
    }
    min_.set(0,0), max_.set(width()-1,height()-1);
}

template <typename PixelT>
void canvas<PixelT, pixmap>::clear(const vec3b& rgb) {
    for(int y = 0, yend = height()-1; y <= yend; ++y) {
        for(int x = 0, xend = width()-1; x <= xend; ++x) {
            raster_(x,y).set3(rgb);
        }
    }
    min_.set(0,0), max_.set(width()-1,height()-1);
}

template <typename PixelT>
void canvas<PixelT, pixmap>::clear() {
    for(int y = 0, yend = height()-1; y <= yend; ++y) {
        for(int x = 0, xend = width()-1; x <= xend; ++x) {
            raster_(x,y).set3(clear_colour_);
        }
    }
    min_.set(0,0), max_.set(width()-1,height()-1);
}

template <typename PixelT>
void canvas<PixelT, pixmap>::line_impl(int x1, int y1, int x2, int y2) {
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

template <typename PixelT>
void canvas<PixelT, pixmap>::circle(int cx, int cy, int r) {
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

template <typename PixelT>
void canvas<PixelT, pixmap>::ellipse(int cx, int cy, int major, int minor) {
    update_region(cx-major,cy-minor); update_region(cx+major,cy+minor);

    const int major2 = 2*major*major, minor2 = 2*minor*minor;
    int x = -major, y = 0;
    int e2 = minor, dx = (1+2*x)*e2*e2;
    int dy = x*x, err = dx+dy;
    do {
        ellipse_points(cx, cy, x, y);
        e2 = 2*err;
        if(e2 >= dx) { x++; dx += minor2; err += dx; }
        if(e2 <= dy) { y++; dy += major2; err += dy; }
    } while(x <= 0);

    while(y++ < minor) ellipse_points(cx, cy, x, y);
}

template <typename PixelT>
void canvas<PixelT, pixmap>::rect(int x1, int y1, int x2, int y2) {
    update_region(x1,y1); update_region(x2,y2);
    int left, bottom, right, top;
    if(x1 < x2) { left = x1; right = x2; }
    else        { left = x2; right = x1; }
    if(y1 < y2) { bottom = y1; top = y2; }
    else        { bottom = y2; top = y1; }

    // Use principle of bottom and left being part of primitive, top and right, not.
    line_impl(left, bottom, right, bottom);
    line_impl(right, bottom, right, top);
    line_impl(left, top, right, top);
    line_impl(left, bottom, left, top);
}

template <typename PixelT>
void canvas<PixelT, pixmap>::filled_rect(int x1, int y1, int x2, int y2) {
    update_region(x1,y1); update_region(x2,y2);
    int left, bottom, right, top;
    if(x1 < x2) { left = x1; right = x2; }
    else        { left = x2; right = x1; }
    if(y1 < y2) { bottom = y1; top = y2; }
    else        { bottom = y2; top = y1; }

    if(right-left <= 0) return;

    // Use principle of bottom and left being part of primitive, top and right, not.
    for(int c = left; c < right; ++c) raster_(c,bottom).set3(fill_colour_);

    const auto diff = right - left;
    for(int r = bottom+1; r < top; ++r) {
        raster_.copy(raster_.offset(left,bottom), raster_.offset(left,r), diff);
    }
}

template <typename PixelT>
size_t canvas<PixelT, pixmap>::copy(const canvas<PixelT, pixmap>::pixmap_t& src, int trg_x, int trg_y, const recti& bound) {
    return raster_.copy(src, trg_x, trg_y, bound);
}

template <typename PixelT>
void canvas<PixelT, pixmap>::vertical_line(int x1, int y1, int y2) {
    if(y2 < y1) std::swap(y1, y2); //assert(y1 < y2 && "vertical_line requires y1 < y2");
    int y = y1;
    while(y <= y2) raster_(x1,y++).set3(pen_colour_);
}

template <typename PixelT>
void canvas<PixelT, pixmap>::horizontal_line(int x1, int x2, int y1) {
    assert(x1 < x2 && "horizontal_line requires x1 < x2");
    int x = x1;
    while(x <= x2) raster_(x++,y1).set3(pen_colour_);
}

template <typename PixelT>
void canvas<PixelT, pixmap>::diagonal_line(int x1, int y1, int x2, int y2) {
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

template <typename PixelT>
void canvas<PixelT, pixmap>::circle_points(int cx, int cy, int x, int y) {
    raster_(cx + x, cy + y).set3(pen_colour_); raster_(cx + x, cy - y).set3(pen_colour_);
    raster_(cx - x, cy + y).set3(pen_colour_); raster_(cx - x, cy - y).set3(pen_colour_);
    raster_(cx + y, cy + x).set3(pen_colour_); raster_(cx + y, cy - x).set3(pen_colour_);
    raster_(cx - y, cy + x).set3(pen_colour_); raster_(cx - y, cy - x).set3(pen_colour_);
}

template <typename PixelT>
void canvas<PixelT, pixmap>::ellipse_points(int cx, int cy, int x, int y) {
    raster_(cx + x, cy + y).set3(pen_colour_); raster_(cx - x, cy + y).set3(pen_colour_);
    raster_(cx + x, cy - y).set3(pen_colour_); raster_(cx - x, cy - y).set3(pen_colour_);
}

// Cohen-Sutherland line clipper
template <typename PixelT>
void canvas<PixelT, pixmap>::line(int x1, int y1, int x2, int y2) {
    auto region = [](int x, int y, int xmin, int xmax, int ymin, int ymax) {
        int code = 0;
        if(y > ymax)      code |= clip_plane::CP_TOP;
        else if(y < ymin) code |= clip_plane::CP_BOTTOM;
        if(x > xmax)      code |= clip_plane::CP_RIGHT;
        else if(x < xmin) code |= clip_plane::CP_LEFT;
        return code;
    };

    int code1, code2, codeOut;
    bool accept = false, done = false;
    code1 = region(x1, y1, clip_region_.left, clip_region_.right, clip_region_.bottom, clip_region_.top);
    code2 = region(x2, y2, clip_region_.left, clip_region_.right, clip_region_.bottom, clip_region_.top);
    do {
        if(!(code1 | code2)) { accept = true; done = true; }        // Whole line contained
        else if(code1 & code2) done = true;                         // Whole line rejected
        else {
            int x=0, y=0;
            codeOut = code1 ? code1 : code2;
            if(codeOut & CP_TOP) {
                x = x1 + (x2 - x1) * (clip_region_.top - y1) / (y2 - y1);
                y = clip_region_.top;
            } else if(codeOut & CP_BOTTOM) {
                x = x1 + (x2 - x1) * (clip_region_.bottom - y1) / (y2 - y1);
                y = clip_region_.bottom;
            } else if(codeOut & CP_RIGHT) {
                y = y1 + (y2 - y1) * (clip_region_.right - x1) / (x2 - x1);
                x = clip_region_.right;
            } else if(codeOut & CP_LEFT) {
                y = y1 + (y2 - y1) * (clip_region_.left - x1) / (x2 - x1);
                x = clip_region_.left;
            }

            if(codeOut == code1) {
                x1 = x; y1 = y;
                code1 = region(x1, y1, clip_region_.left, clip_region_.right, clip_region_.bottom, clip_region_.top);
            } else {
                x2 = x; y2 = y;
                code2 = region(x2, y2, clip_region_.left, clip_region_.right, clip_region_.bottom, clip_region_.top);
            }
        }
    } while(!done);

    if(accept) line_impl(x1, y1, x2, y2);
}

template <typename PixelT>
void canvas<PixelT, pixmap>::polyline(const std::vector<segment2i>& polyline) {
    for(const auto& l : polyline) {
        line(l.P0, l.P1);
    }
}

template <typename PixelT>
void canvas<PixelT, pixmap>::polyline(const std::vector<vec2i>& polyline) {
    auto size = polyline.size();
    if(size < 2) return;
    for(size_t i = 0, j = 1; j != size; i = j, ++j) line(polyline[i], polyline[j]);
}

template <typename PixelT>
void canvas<PixelT, pixmap>::polyloop(const std::vector<vec2i>& polyloop) {
    auto size = polyloop.size();
    if(size < 2) return;
    for(size_t i = 0, j = 1; j != size; i = j, ++j) line(polyloop[i], polyloop[j]);
    size--;
    line(polyloop[size], polyloop[0]);
}

template <typename PixelT>
void canvas<PixelT, pixmap>::polygon(const std::vector<vec2i>& polygon) {
    if(polygon.size() < 3) { LOG_ERR("Polygon must consist of at least three vertices"); return; }
    edge_table global_et(polygon);
    if(global_et.buckets.size() == 0) { LOG_ERR("Edge Table empty"); return; }

    std::vector<edge_table::edge> AET;

    int curr_bucket = 0;
    int curr_y = global_et.min_y;
    while(curr_y != global_et.max_y) {
        while(global_et.buckets[curr_bucket].y == curr_y) {
            auto& current = global_et.buckets[curr_bucket];
            std::copy(current.edges.begin(), current.edges.end(), std::back_inserter(AET));
            ++curr_bucket;
        }

        auto it = std::remove_if(std::begin(AET), std::end(AET), [&curr_y](const edge_table::edge& ed) {
            return ed.ymax == curr_y;
        });
        if(it != std::end(AET)) AET.erase(it, AET.end());
        if(AET.size() % 2 != 0) { LOG_ERR("AET contains non-even number of edges"); return; }

        std::sort(std::begin(AET), std::end(AET), [](const edge_table::edge& A, const edge_table::edge& B) {
            return A.xmin < B.xmin;
        });

        for(int i = 0, end = (int)AET.size(); i != end; i += 2) {
            auto& se = AET[2*i]; auto& ee = AET[2*i+1];
            for(int x = se.xmin, xend = ee.xmin; x <= xend; ++x) {
                raster_(x,curr_y).set3(fill_colour_);
            }
            if(se.denominator != 0) {
                se.increment += se.numerator;
                while(se.increment > se.denominator) {
                    se.xmin += se.sign; se.increment -= se.denominator;
                }
            }
            if(ee.denominator != 0) {
                ee.increment += ee.numerator;
                while(ee.increment > ee.denominator) {
                    ee.xmin += ee.sign; ee.increment -= ee.denominator;
                }
            }
        }

        ++curr_y;
    }
}

template class zap::rasteriser::canvas<rgb888_t, pixel_buffer>;
template class zap::rasteriser::canvas<rgba8888_t, pixel_buffer>;
template class zap::rasteriser::canvas<rgb888_t, pixmap>;
//template class zap::rasteriser::canvas<rgba8888_t, pixmap>;
