//
// Created by Darren Otgaar on 2016/09/17.
//

#ifndef ZAP_RECT_HPP
#define ZAP_RECT_HPP

#include <maths/vec2.hpp>

namespace zap { namespace maths { namespace geometry {

template <typename T>
struct rect {
    T left, right, bottom, top;
    rect() = default;
    rect(const T& left, const T& right, const T& bottom, const T& top) : left(left), right(right), bottom(bottom),
                                                                         top(top) { }
    rect(const vec2<T> horz, const vec2<T> vert) : left(horz.x), right(horz.y), bottom(vert.x), top(vert.y) { }

    void set(const T& left, const T& right, const T& bottom, const T& top) {
        this->left = left; this->right = right; this->bottom = bottom; this->top = top;
    }

    bool intersection(const vec2<T>& P) const {
        return left <= P.x && P.x <= right && bottom <= P.y && P.y <= top;
    }

    vec2<T> centre() const { return vec2<T>((right-left)/2, (top-bottom)/2); }
    vec2<T> bl() const { return vec2<T>(left, bottom); }
    vec2<T> tl() const { return vec2<T>(left, top); }
    vec2<T> br() const { return vec2<T>(right, bottom); }
    vec2<T> tr() const { return vec2<T>(right, top); }
};

using recti = rect<int>;
using rectf = rect<float>;

}}}

#endif //ZAP_RECT_HPP
