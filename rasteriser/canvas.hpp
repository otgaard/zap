/* Created by Darren Otgaar on 2016/08/20. http://www.github.com/otgaard/zap */
#ifndef ZAP_CANVAS_HPP
#define ZAP_CANVAS_HPP

// A 2D Software Rasteriser for rasterising lines, circles, ellipses, text, using clipping & ranged updates and Pixel
// Buffer usage in OpenGL.  The basis for a small vector graphics engine.

#include <maths/vec2.hpp>
#include <maths/vec3.hpp>
#include <engine/pixmap.hpp>
#include <engine/texture.hpp>

namespace zap { namespace rasteriser {
    using vec2i = maths::vec2i;
    using vec3b = maths::vec3b;

    class canvas {
    public:
        canvas();
        canvas(int width, int height);
        ~canvas();

        bool map();
        void unmap();

        inline int width() const { return raster_.width(); }
        inline int height() const { return raster_.height(); }
        inline vec2i centre() const { return vec2i(raster_.width()/2, raster_.height()/2); }

        void resize(int width, int height);

        void clear(byte r, byte g, byte b);
        void clear(const vec3b& rgb);
        void clear();

        void pen_colour(const vec3b& rgb) { pen_colour_ = rgb; }
        const vec3b& pen_colour() const { return pen_colour_; }
        void clear_colour(const vec3b& rgb) { clear_colour_ = rgb; }
        const vec3b& clear_colour() const { return clear_colour_; }
        void fill_colour(const vec3b& rgb) { fill_colour_ = rgb; }
        const vec3b& fill_colour() const { return fill_colour_; }

        void line(int x1, int y1, int x2, int y2);
        inline void line(const vec2i& A, const vec2i& B) { line(A.x,A.y,B.x,B.y); }

        void circle(int cx, int cy, int r);
        inline void circle(const vec2i& C, int r) { circle(C.x, C.y, r); }

        void ellipse(int cx, int cy, int major, int minor);
        inline void ellipse(const vec2i& C, int major, int minor) { ellipse(C.x, C.y, major, minor); }

        void rect(int x1, int y1, int x2, int y2);
        void filled_rect(int x1, int y1, int x2, int y2);

        void update(zap::engine::texture& tex);

    protected:
        using rgb888_t = zap::engine::rgb888_t;
        using pbuf_t = zap::engine::pixel_buffer<rgb888_t,zap::engine::buffer_usage::BU_DYNAMIC_DRAW>;

        void initialise();
        void update_region(int x, int y);

        void vertical_line(int x1, int y1, int y2);
        void horizontal_line(int x1, int x2, int y1);
        void diagonal_line(int x1, int y1, int x2, int y2);

        void circle_points(int cx, int cy, int x, int y);
        void ellipse_points(int cx, int cy, int x, int y);

        rgb888_t* mapped_ptr_;
        vec2i min_, max_;
        vec3b pen_colour_, clear_colour_, fill_colour_;
        int block_width_;
        pbuf_t raster_;
        pbuf_t pbo_;
    };
}}

#endif //ZAP_CANVAS_HPP
