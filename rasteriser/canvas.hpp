/* Created by Darren Otgaar on 2016/08/20. http://www.github.com/otgaard/zap */
#ifndef ZAP_CANVAS_HPP
#define ZAP_CANVAS_HPP

// A 2D Software Rasteriser for rasterising lines, circles, ellipses, text, using clipping & ranged updates and Pixel
// Buffer usage in OpenGL.  The basis for a small vector graphics engine.

#include <maths/vec2.hpp>
#include <maths/vec3.hpp>
#include <engine/pixmap.hpp>
#include <engine/texture.hpp>
#include <maths/geometry/rect.hpp>
#include <maths/geometry/segment.hpp>

namespace zap { namespace rasteriser {
    using maths::vec2i;
    using maths::vec3b;
    using maths::geometry::recti;
    using maths::geometry::segment2i;

    template <typename PixelT, template <typename> class BufferT> class canvas;

    template <typename PixelT>
    class canvas<PixelT, engine::pixel_buffer> {
    public:
        canvas();
        canvas(int width, int height);
        ~canvas();

        bool map();
        void unmap();

        int width() const { return int32_t(raster_.width()); }
        int height() const { return int32_t(raster_.height()); }
        vec2i centre() const { return vec2i{int32_t(raster_.width()/2), int32_t(raster_.height()/2)}; }

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
        void line(const vec2i& A, const vec2i& B) { line(A.x, A.y, B.x, B.y); }

        void polyline(const std::vector<segment2i>& polyline);
        void polyline(const std::vector<vec2i>& polyline);
        void polyloop(const std::vector<vec2i>& polyloop);
        void polygon(const std::vector<vec2i>& polygon);

        void circle(int cx, int cy, int r);
        void circle(const vec2i& C, int r) { circle(C.x, C.y, r); }

        void ellipse(int cx, int cy, int major, int minor);
        void ellipse(const vec2i& C, int major, int minor) { ellipse(C.x, C.y, major, minor); }

        void rect(int x1, int y1, int x2, int y2);
        void filled_rect(int x1, int y1, int x2, int y2);

        void update(zap::engine::texture& tex);

    protected:
        using pixel_t = PixelT;
        using pbuf_t = engine::pixel_buffer<PixelT>;

        void initialise();
        void update_region(int x, int y);

        void line_impl(int x1, int y1, int x2, int y2);

        void vertical_line(int x1, int y1, int y2);
        void horizontal_line(int x1, int x2, int y1);
        void diagonal_line(int x1, int y1, int x2, int y2);

        void circle_points(int cx, int cy, int x, int y);
        void ellipse_points(int cx, int cy, int x, int y);


        pixel_t* mapped_ptr_;
        vec2i min_, max_;
        vec3b pen_colour_, clear_colour_, fill_colour_;
        recti clip_region_;
        int block_width_;
        pbuf_t raster_;
        pbuf_t pbo_;
    };

template <typename PixelT>
class canvas<PixelT, engine::pixmap> {
public:
    using pixmap_t = engine::pixmap<PixelT>;

    canvas();
    canvas(int width, int height);
    ~canvas();

    int width() const { return int32_t(raster_.width()); }
    int height() const { return int32_t(raster_.height()); }
    vec2i centre() const { return vec2i{int32_t(raster_.width()/2), int32_t(raster_.height()/2)}; }

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
    void line(const vec2i& A, const vec2i& B) { line(A.x, A.y, B.x, B.y); }

    void polyline(const std::vector<segment2i>& polyline);
    void polyline(const std::vector<vec2i>& polyline);
    void polyloop(const std::vector<vec2i>& polyloop);
    void polygon(const std::vector<vec2i>& polygon);

    void circle(int cx, int cy, int r);
    void circle(const vec2i& C, int r) { circle(C.x, C.y, r); }

    void ellipse(int cx, int cy, int major, int minor);
    void ellipse(const vec2i& C, int major, int minor) { ellipse(C.x, C.y, major, minor); }

    void rect(int x1, int y1, int x2, int y2);
    void filled_rect(int x1, int y1, int x2, int y2);

    size_t copy(const pixmap_t& src, int trg_x, int trg_y, const recti& bound=recti{0, 0, 0, 0});

    void update(zap::engine::texture& tex);
    const engine::pixmap<PixelT>& get_buffer() const { return raster_; }

protected:
    using pixel_t = PixelT;
    using pbuf_t = engine::pixmap<PixelT>;

    void initialise();
    void update_region(int x, int y);

    void line_impl(int x1, int y1, int x2, int y2);

    void vertical_line(int x1, int y1, int y2);
    void horizontal_line(int x1, int x2, int y1);
    void diagonal_line(int x1, int y1, int x2, int y2);

    void circle_points(int cx, int cy, int x, int y);
    void ellipse_points(int cx, int cy, int x, int y);

    pixel_t* mapped_ptr_;
    vec2i min_, max_;
    vec3b pen_colour_, clear_colour_, fill_colour_;
    recti clip_region_;
    pbuf_t raster_;
};

}}

#endif //ZAP_CANVAS_HPP
