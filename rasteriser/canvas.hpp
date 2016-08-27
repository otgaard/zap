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

    namespace ze = zap::engine;

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

        void clear(byte r=255, byte g=255, byte b=255);
        void clear(const vec3b& rgb);

        void pen_colour(const vec3b& rgb) { pen_colour_ = rgb; }
        const vec3b& pen_colour() const { return pen_colour_; }

        // Lines
        void line(int x1, int y1, int x2, int y2);
        inline void line(const vec2i& A, const vec2i& B) { line(A.x,A.y,B.x,B.y); }

        // Circles
        void circle(int cx, int cy, int r);

        //const std::vector<zap::engine::rgb888_t>& buffer() const { return raster_.buffer(); }
        const ze::pixel_buffer<ze::rgb888_t,ze::buffer_usage::BU_STREAM_DRAW>& buffer() const { return raster_; }

        void update(zap::engine::texture& tex);

    protected:

        void initialise();
        void update_region(int x, int y);

        void vertical_line(int x1, int y1, int y2);
        void horizontal_line(int x1, int x2, int y1);
        void diagonal_line(int x1, int y1, int x2, int y2);

        void circle_points(int cx, int cy, int x, int y);

        ze::rgb888_t* mapped_ptr_;
        vec2i min_, max_;
        vec3b pen_colour_;

        //bool is_mapped_;
        //zap::engine::pixmap<zap::engine::rgb888_t> raster_;

        ze::pixel_buffer<ze::rgb888_t,ze::buffer_usage::BU_STREAM_DRAW> raster_;
        ze::pixel_buffer<ze::rgb888_t,ze::buffer_usage::BU_DYNAMIC_DRAW> pbo_;
    };
}}

#endif //ZAP_CANVAS_HPP
