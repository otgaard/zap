//
// Created by Darren Otgaar on 2016/09/17.
//

#ifndef ZAP_RECT_HPP
#define ZAP_RECT_HPP

#include <maths/vec2.hpp>

namespace zap { namespace maths { namespace geometry {

template <typename T>
struct rect {
    rect() = default;
    rect(const rect& rhs) = default;
    rect(const T& left, const T& right, const T& bottom, const T& top) :
            left(left), right(right), bottom(bottom), top(top) { }
    rect(const vec2<T> horz, const vec2<T> vert) : left(horz.x), right(horz.y), bottom(vert.x), top(vert.y) { }

    rect& operator=(const rect& rhs) = default;

    void set(const T& left, const T& right, const T& bottom, const T& top) {
        this->left = left; this->right = right; this->bottom = bottom; this->top = top;
    }

    T width() const { return right - left; }
    T height() const { return bottom - top; }

    void translate(T x, T y) {
        left += x; right += x;
        bottom += y; top += y;
    }

    void translate(const vec2<T>& v) { translate(v.x, v.y); }

    bool intersection(T x, T y) const { return left <= x && x <= right && bottom <= y && y <= top; }
    bool intersection(const vec2<T>& P) const { return intersection(P.x, P.y); }
    bool intersection(const rect& rhs, rect* overlap=nullptr) const {
        rect test{
                std::max(left, rhs.left), std::min(right, rhs.right),
                std::min(bottom, rhs.bottom), std::max(top, rhs.top)
        };
        if(test.left < test.right && test.bottom < test.top) {
            if(overlap) *overlap = test;
            return true;
        }
        return false;
    }

    vec2<T> centre() const { return vec2<T>((right-left)/2, (top-bottom)/2); }
    vec2<T> bl() const { return vec2<T>(left, bottom); }
    vec2<T> tl() const { return vec2<T>(left, top); }
    vec2<T> br() const { return vec2<T>(right, bottom); }
    vec2<T> tr() const { return vec2<T>(right, top); }

    T left, right, bottom, top;
};

using recti = rect<int>;
using rectf = rect<float>;

}}}

#endif //ZAP_RECT_HPP
