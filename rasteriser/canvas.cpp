/* Created by Darren Otgaar on 2016/08/20. http://www.github.com/otgaard/zap */
#include <renderer/colour.hpp>
#include "canvas.hpp"

#include <maths/io.hpp>

using namespace zap::engine;
using namespace zap::rasteriser;

canvas::canvas() : mapped_ptr_(nullptr) {
}

canvas::canvas(int width, int height) : mapped_ptr_(nullptr), clip_region_(0, width-1, 0, height-1) {
    initialise();
}

canvas::~canvas() {
}

bool canvas::map() {
    if(mapped_ptr_) return true;

    min_.x = width(); min_.y = height();
    max_.x = 0;       max_.y = 0;
    raster_.bind();
    mapped_ptr_ = reinterpret_cast<rgb888_t*>(raster_.map(buffer_access::BA_WRITE_ONLY, true));
    return mapped_ptr_ != nullptr;
}

void canvas::unmap() {
    raster_.unmap(true);
    raster_.release();
    mapped_ptr_ = nullptr;
    min_.x = maths::clamp(min_.x, 0, width()); max_.x = maths::clamp(max_.x, 0, width());
    min_.y = maths::clamp(min_.y, 0, height()); max_.y = maths::clamp(max_.y, 0, height());
}

void canvas::initialise() {
    pen_colour_ = colour::black8;
    fill_colour_ = colour::white8;
    clear_colour_ = colour::white8;
}

void canvas::update(texture& tex) {
    if(mapped_ptr_) { LOG("Error, unmap canvas to update texture"); return; }

    max_.x += 1; max_.y += 1;
    auto w = max_.x - min_.x, h = max_.y - min_.y;
    if(w > 0 && h > 0) {
        if(tex.width() != width() || tex.height() != height()) {
            tex.initialise(texture_type::TT_TEX2D,width(),height(),pixel_format::PF_RGB, pixel_datatype::PD_UNSIGNED_BYTE);
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

inline void canvas::update_region(int x, int y) {
    if(x < min_.x)  min_.x = x;
    if(x > max_.x)  max_.x = x;
    if(y < min_.y)  min_.y = y;
    if(y > max_.y)  max_.y = y;
}

void canvas::resize(int width, int height) {
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

void canvas::clear() {
    for(int y = 0, yend = height()-1; y <= yend; ++y) {
        for(int x = 0, xend = width()-1; x <= xend; ++x) {
            raster_(x,y).set3(clear_colour_);
        }
    }
    min_.set(0,0), max_.set(width()-1,height()-1);
}

void canvas::line_impl(int x1, int y1, int x2, int y2) {
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

void canvas::ellipse(int cx, int cy, int major, int minor) {
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

void canvas::rect(int x1, int y1, int x2, int y2) {
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

void canvas::filled_rect(int x1, int y1, int x2, int y2) {
    update_region(x1,y1); update_region(x2,y2);
    int left, bottom, right, top;
    if(x1 < x2) { left = x1; right = x2; }
    else        { left = x2; right = x1; }
    if(y1 < y2) { bottom = y1; top = y2; }
    else        { bottom = y2; top = y1; }

    if(right-left <= 0) return;

    // Use principle of bottom and left being part of primitive, top and right, not.
    for(int c = left; c <= right; ++c) raster_(c,bottom).set3(fill_colour_);

    const auto diff = right - left;
    for(int r = bottom+1; r < top; ++r) {
        raster_.copy(raster_.idx(left,bottom), raster_.idx(left,r), diff);
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

void canvas::ellipse_points(int cx, int cy, int x, int y) {
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
void canvas::line(int x1, int y1, int x2, int y2) {
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
            int x, y;
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

void canvas::polyline(const std::vector<segment2i>& polyline) {
    for(const auto& l : polyline) {
        line(l.P0, l.P1);
    }
}

void canvas::polyline(const std::vector<vec2i>& polyline) {
    auto size = polyline.size();
    if(size < 2) return;
    for(int i = 0, j = 1; j != size; i = j, ++j) line(polyline[i], polyline[j]);
}

void canvas::polyloop(const std::vector<vec2i>& polyloop) {
    auto size = polyloop.size();
    if(size < 2) return;
    for(int i = 0, j = 1; j != size; i = j, ++j) line(polyloop[i], polyloop[j]);
    size--;
    line(polyloop[size], polyloop[0]);
}

// Global Edge Table for Polygon Filling (see page 98 in CGPAP 2nd)

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

        zap::maths::range_finder<int> yrange;
        for(const auto& P : polygon) yrange(P.y);
        min_y = yrange.min; max_y = yrange.max;
        assert(min_y < max_y && "Min must be less than Max");
        LOG("minmax:", yrange.min, yrange.max);

        std::sort(vl.begin(), vl.end(), [](const vec2i& A, const vec2i& B) {
            return (A.y < B.y) || (A.y == B.y && A.x < B.x);
        });

        int counter = 0;
        while(counter != vl.size()) {
            auto it = std::find(polygon.begin(), polygon.end(), vl[counter]);
            if(it->y == yrange.max) break; // We must be at the last vertex or one of the last vertices

            LOG("S:", *it);

            auto i = it - polygon.begin();
            auto left = i == 0 ? int(polygon.size())-1 : i-1, right = i == polygon.size()-1 ? 0 : i+1;
            if(polygon[left].x > polygon[right].x) std::swap(left,right);

            LOG(polygon[left], polygon[right]);
            if(polygon[left].y < it->y && polygon[right].y < it->y) continue;
            bucket nb;
            nb.y = it->y;
            if(polygon[left].y > nb.y) {
                auto& l = polygon[left];
                edge e1;
                e1.ymax = l.y;
                e1.numerator = l.x - it->x;
                e1.denominator = l.y - it->y;
                e1.sign = zap::maths::sign(e1.numerator*e1.denominator);
                e1.increment = std::abs(e1.denominator);
                e1.xmin = it->x;
                nb.edges.emplace_back(std::move(e1));
            }

            if(polygon[right].y > nb.y) {
                auto& r = polygon[right];
                edge e2;
                e2.ymax = r.y;
                e2.numerator = r.x - it->x;
                e2.denominator = r.y - it->y;
                e2.sign = zap::maths::sign(e2.numerator*e2.denominator);
                e2.increment = std::abs(e2.denominator);
                e2.xmin = it->x;
                nb.edges.emplace_back(std::move(e2));
            }
            buckets.push_back(nb);
            ++counter;
        }

        for(auto& b : buckets) {
            LOG("LINE:", b.y);
            for(auto& e : b.edges) {
                LOG("   EDGE:", e.ymax, e.xmin, e.numerator, e.denominator);
            }
        }
    }
};

void canvas::left_edge_scan(int xmin, int ymin, int xmax, int ymax) {
    int x = xmin;
    int numerator = xmax - xmin;
    int denominator = ymax - ymin;
    int increment = denominator;

    for(int y = ymin; y <= ymax; ++y) {
        for(int c = x; c != xmax; ++c) raster_(c,y).set3(fill_colour_);
        increment += numerator;
        if(increment > denominator) {
            ++x; increment -= denominator;
        }
    }
}

void canvas::polygon(const std::vector<vec2i>& polygon) {
    if(polygon.size() < 3) { LOG_ERR("Polygon must consist of at least three vertices"); return; }
    edge_table global_et(polygon);
    if(global_et.buckets.size() == 0) { LOG_ERR("Edge Table empty"); return; }

    std::vector<edge_table::edge> AET;

    int curr_bucket = 0;
    int curr_y = global_et.min_y;
    while(curr_y != global_et.max_y) {
        if(global_et.buckets[curr_bucket].y == curr_y) {
            std::copy(global_et.buckets[0].edges.begin(), global_et.buckets[0].edges.end(),
                      std::back_inserter(AET));
        }

        auto it = std::remove_if(std::begin(AET), std::end(AET),
                                 [&curr_y](const edge_table::edge& ed) {
                                    return ed.ymax == curr_y;
                                 });
        if(it != std::end(AET)) AET.erase(it);

        std::sort(std::begin(AET), std::end(AET),
                  [](const edge_table::edge& A, const edge_table::edge& B) {
                      return A.xmin < B.xmin;
                  });

        if(AET.size() % 2 != 0) {



            LOG_ERR("AET contains non-even number of edges");
            return;
        }

        for(int i = 0, end = (int)AET.size(); i != end; i += 2) {
            auto& se = AET[2*i]; auto& ee = AET[2*i+1];
            for(int x = se.xmin, xend = ee.xmin; x <= xend; ++x) raster_(x,curr_y).set3(fill_colour_);
            se.increment += se.numerator;
            if(se.increment > se.denominator) {
                se.xmin += se.sign; se.increment -= se.denominator;
            }
            ee.increment += ee.numerator;
            if(ee.increment > ee.denominator) {
                ee.xmin += ee.sign; ee.increment -= ee.denominator;
            }
        }

        ++curr_y;
    }
}
