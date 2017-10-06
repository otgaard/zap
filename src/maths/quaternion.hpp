//
// Created by Darren Otgaar on 2017/09/28.
//

#ifndef ZAP_QUATERNION_HPP
#define ZAP_QUATERNION_HPP

#include "maths.hpp"
#include "vec3.hpp"
#include "vec4.hpp"

namespace zap { namespace maths {
    template <typename T>
    class quaternion {
    public:
        using type = T;

        quaternion() = default;
        ~quaternion() = default;
        explicit quaternion(T init);
        quaternion(T w, T x, T y, T z);
        quaternion(const vec3<T>& from, const vec3<T>& to);
        quaternion(const vec3<T>& axis, T angle);
        explicit quaternion(const vec3<T>& v);
        explicit quaternion(const mat3<T>& rot);
        quaternion(const quaternion& p);
        quaternion& operator=(const quaternion& p);

        bool operator==(const quaternion& p) const;
        bool operator!=(const quaternion& p) const;
        bool eq(const quaternion& p) const;
        bool neq(const quaternion& p) const;
        bool is_zero() const;
        bool is_unit() const;
        bool is_identity() const;

        void set(T w, T x, T y, T z);
        void set(const vec3<T>& axis, T angle);
        void set(const vec3<T>& from, const vec3<T>& to);
        void set(const mat3<T>& rot);
        void set(T z_rot, T y_rot, T x_rot);
        mat3<T> rotation() const;
        void rotation(mat3<T>& m) const;

        quaternion operator-() const;
        quaternion& operator*=(const quaternion& q);
        quaternion& operator*=(T s);
        quaternion& operator+=(const quaternion& q);
        quaternion& operator+=(const vec3<T>& v);
        quaternion& operator-=(const quaternion& q);
        quaternion& operator-=(const vec3<T>& v);

        quaternion& clean();
        quaternion& clear();
        T norm_sq() const;
        T norm() const;
        T length_sq() const;
        T length() const;
        void identity();
        quaternion& invert();
        quaternion& normalise();
        T dot(const quaternion& q) const;

        const T* data() const { return &q_[0]; }
        T operator[](size_t idx) const { assert(idx<4); return q_[idx]; }
        T& operator[](size_t idx) { assert(idx<4); return q_[idx]; }
        T w() const { return q_[0]; }
        T& w() { return q_[0]; }
        void w(T w) { q_[0] = w; }
        T x() const { return q_[1]; }
        T& x()  { return q_[1]; }
        void x(T x) { q_[1] = x; }
        T y() const { return q_[2]; }
        T& y() { return q_[2]; }
        void y(T y)                    { q_[2] = y; }
        T z() const                    { return q_[3]; }
        T& z()                         { return q_[3]; }
        void z(T z)                    { q_[3] = z; }

    protected:
        T q_[4];
    };

    ///////////////////////////////////////////////////////////////////////////
    // Typedefs
    typedef quaternion<float>  quatf;
    typedef quaternion<double> quatd;

    ///////////////////////////////////////////////////////////////////////////
    // Function Declarations

    template <typename T>
    quaternion<T> inverse(const quaternion<T>& q);
    template <typename T>
    quaternion<T> operator*(const quaternion<T>& p, const quaternion<T>& q);
    template <typename T>
    vec3<T> operator*(const quaternion<T>& q, const vec3<T>& v);
    template <typename T>
    quaternion<T> operator*(const quaternion<T>& p, T s);
    template <typename T>
    quaternion<T> operator*(T s, const quaternion<T>& p);
    template <typename T>
    quaternion<T> operator+(const quaternion<T>& p, const quaternion<T>& q);
    template <typename T>
    quaternion<T> operator-(const quaternion<T>& p, const quaternion<T>& q);
    template <typename T>
    quaternion<T> slerp(T t, const quaternion<T>& start, const quaternion<T>& end);

    ///////////////////////////////////////////////////////////////////////////
    // Method Implementation

    template <typename T>
    quaternion<T>::quaternion(T init) {
        w() = init; x() = init; y() = init; z() = init;
    }

    template <typename T>
    quaternion<T>::quaternion(T w, T x, T y, T z) {
        q_[0] = w; q_[1] = x; q_[2] = y; q_[3] = z;
    }

    template <typename T>
    quaternion<T>::quaternion(const vec3<T>& from, const vec3<T>& to) {
        set(from, to);
    }

    template <typename T>
    quaternion<T>::quaternion(const vec3<T>& axis, T angle) {
        set(axis, angle);
    }

    template <typename T>
    quaternion<T>::quaternion(const vec3<T>& v) {
        set(v);
    }

    template <typename T>
    quaternion<T>::quaternion(const mat3<T>& rot) {
        set(rot);
    }

    template <typename T>
    quaternion<T>::quaternion(const quaternion& p) {
        w() = p.w(); x() = p.x(); y() = p.y(); z() = p.z();
    }

    template <typename T>
    quaternion<T>& quaternion<T>::operator=(const quaternion<T>& p) {
        if(this != &p) {
            w() = p.w(); x() = p.x(); y() = p.y(); z() = p.z();
        }
        return *this;
    }

    template <typename T>
    bool quaternion<T>::operator==(const quaternion& p) const {
        return w() == p.w() && x() == p.x() && y() == p.y() && z() == p.z();
    }

    template <typename T>
    bool quaternion<T>::operator!=(const quaternion& p) const {
        return !operator==(p);
    }

    template <typename T>
    bool quaternion<T>::eq(const quaternion& p) const {
        return maths::eq(w(),p.w()) && maths::eq(x(),p.x()) && maths::eq(y(),p.y()) && maths::eq(z(),p.z());
    }

    template <typename T>
    bool quaternion<T>::neq(const quaternion& p) const {
        return !eq(p);
    }

    template <typename T>
    bool quaternion<T>::is_zero() const {
        return maths::is_zero(w()) && maths::is_zero(x()) && maths::is_zero(y()) && maths::is_zero(z());
    }

    template <typename T>
    bool quaternion<T>::is_unit() const {
        return maths::eq(norm_sq(), (T)1);
    }

    template <typename T>
    bool quaternion<T>::is_identity() const {
        return maths::eq(w(), (T)1) && maths::is_zero(x()) && maths::is_zero(y()) && maths::is_zero(z());
    }

    template <typename T>
    void quaternion<T>::set(T w, T x, T y, T z) {
        w() = w; x() = x; y() = y; z() = z;
    }

    template <typename T>
    void quaternion<T>::set(const vec3<T>& axis, T angle) {
        float n = axis.length_sqr();
        if(maths::is_zero(n)) {
            identity();
            return;
        }

        angle *= (T)0.5;

        float sin_theta = sin(angle);
        float cos_theta = cos(angle);

        float s = sin_theta / sqrt(n);
        w() = cos_theta; x() = s*axis.x(); y() = s*axis.y(); z() = s*axis.z();
    }

    template <typename T>
    void quaternion<T>::set(const vec3<T>& from, const vec3<T>& to) {
    }

    template <typename T>
    void quaternion<T>::set(const mat3<T>& rot) {
        float trace = rot.trace();
        if(trace > 0.0f) {
            float s = sqrt(trace + (T)1);
            w() = s*(T)0.5;
            float recip = (T)0.5/s;
            x() = (rot(2,1)-rot(1,2))*recip;
            y() = (rot(0,2)-rot(2,0))*recip;
            z() = (rot(1,0)-rot(0,1))*recip;
        } else {
            uint i = 0;
            if(rot(1,1) > rot(0,0)) i = 1;
            if(rot(2,2) > rot(i,i)) i = 2;

            uint j = (i+1)%3;
            uint k = (j+1)%3;

            float s = sqrt(rot(i,i)-rot(j,j)-rot(k,k)+(T)1);
            (*this)[i] = (T)0.5*s;
            float recip = (T)0.5/s;
            w() = (rot(k,j) - rot(j,k))*recip;
            (*this)[j] = (rot(j,i) + rot(i,j))*recip;
            (*this)[k] = (rot(k,i) + rot(i,k))*recip;
        }
    }

    template <typename T>
    void quaternion<T>::set(T z_rot, T y_rot, T x_rot) {
    }

    template <typename T>
    mat3<T> quaternion<T>::rotation() const {
        mat3<T> m;
        rotation(m);
        return m;
    }

    template <typename T>
    void quaternion<T>::rotation(mat3<T>& m) const {
        assert(is_unit());

        float xs, ys, zs, wx, wy, wz, xx, xy, xz, yy, yz, zz;

        xs = x()+x(); ys = y()+y(); zs = z()+z();
        wx = w()*xs;  wy = w()*ys;  wz = w()*zs;
        xx = x()*xs;  xy = x()*ys;  xz = x()*zs;
        yy = y()*ys;  yz = y()*zs;  zz = z()*zs;

        m[0] = 1.0f - (yy + zz);  m[3] = xy - wz;           m[6] = xz + wy;
        m[1] = xy + wz;           m[4] = 1.0f - (xx + zz);  m[7] = yz - wx;
        m[2] = xz - wy;           m[5] = yz + wx;           m[8] = 1.0f - (xx + yy);
    }

    template <typename T>
    quaternion<T> quaternion<T>::operator-() const {
        return quaternion(-w(), -x(), -y(), -z());
    }

    template <typename T>
    quaternion<T>& quaternion<T>::operator*=(const quaternion<T>& q) {
        (*this) = (*this) * q;
        return (*this);
    }

    template <typename T>
    quaternion<T>& quaternion<T>::operator*=(T s) {
        w() *= s;
        x() *= s;
        y() *= s;
        z() *= s;
        return *this;
    }

    template <typename T>
    quaternion<T>& quaternion<T>::operator+=(const quaternion<T>& q) {
        w() += q.w();
        x() += q.x();
        y() += q.y();
        z() += q.z();
        return *this;
    }

    template <typename T>
    quaternion<T>& quaternion<T>::operator+=(const vec3<T>& v) {
        quaternion q(0, v.x(), v.y(), v.z());
        q *= *this;
        w() += q.w() * ((T)0.5);
        x() += q.x() * ((T)0.5);
        y() += q.y() * ((T)0.5);
        z() += q.z() * ((T)0.5);
        return *this;
    }

    template <typename T>
    quaternion<T>& quaternion<T>::operator-=(const quaternion<T>& q) {
        w() -= q.w();
        x() -= q.x();
        y() -= q.y();
        z() -= q.z();
        return *this;
    }

    template <typename T>
    quaternion<T>& quaternion<T>::operator-=(const vec3<T>& v) {
        quaternion q(0, v.x(), v.y(), v.z());
        q *= *this;
        w() -= q.w() * ((T)0.5);
        x() -= q.x() * ((T)0.5);
        y() -= q.y() * ((T)0.5);
        z() -= q.z() * ((T)0.5);
        return *this;
    }

    template <typename T>
    quaternion<T>& quaternion<T>::clean() {
        if(is_zero(w())) w() = 0;
        if(is_zero(x())) x() = 0;
        if(is_zero(y())) y() = 0;
        if(is_zero(z())) z() = 0;
        return *this;
    }

    template <typename T>
    quaternion<T>& quaternion<T>::clear() {
        w() = 0; x() = 0; y() = 0; z() = 0;
        return *this;
    }

    template <typename T>
    T quaternion<T>::norm_sq() const {
        return w()*w()+x()*x()+y()*y()+z()*z();
    }

    template <typename T>
    T quaternion<T>::norm() const {
        return sqrt(norm_sq());
    }

    template <typename T>
    T quaternion<T>::length_sq() const {
        return norm_sq();
    }

    template <typename T>
    T quaternion<T>::length() const {
        return norm();
    }

    template <typename T>
    void quaternion<T>::identity() {
        w() = (T)1; x() = 0; y() = 0; z() = 0;
    }

    template <typename T>
    quaternion<T>& quaternion<T>::invert() {
        x() = -x(); y() = -y(); z() = -z();
        return *this;
    }

    template <typename T>
    quaternion<T>& quaternion<T>::normalise() {
        float n = norm_sq();
        if(maths::is_zero(n)) clear();
        else {
            n = (T)1/sqrt(n);
            w() *= n; x() *= n; y() *= n; z() *= n;
        }
        return *this;
    }

    template <typename T>
    T quaternion<T>::dot(const quaternion<T>& q) const {
        return w()*q.w() + x()*q.x() + y()*q.y() + z()*q.z();
    }

    ///////////////////////////////////////////////////////////////////////////
    // Function Declarations
    template <typename T>
    quaternion<T> inverse(const quaternion<T>& q) {
        return quaternion<T>(q.w(), -q.x(), -q.y(), -q.z());
    }

    template <typename T>
    quaternion<T> operator*(const quaternion<T>& p, const quaternion<T>& q) {
        quaternion<T> r;

        r.w() = p.w()*q.w() - p.x()*q.x() - p.y()*q.y() - p.z()*q.z();
        r.x() = p.w()*q.x() + p.x()*q.w() + p.y()*q.z() - p.z()*q.y();
        r.y() = p.w()*q.y() + p.y()*q.w() + p.z()*q.x() - p.x()*q.z();
        r.z() = p.w()*q.z() + p.z()*q.w() + p.x()*q.y() - p.y()*q.x();

        return r;
    }

    template <typename T>
    vec3<T> operator*(const quaternion<T>& q, const vec3<T>& v) {
        assert(maths::eq(q.length_sq(),(T)1));

        T vm = (T)2*(q.x()*v.x() + q.y()*v.y() + q.z()*v.z());
        T cm = (T)2*q.w();
        T pm = cm*q.w() - (T)1;

        return vec3<T>(
                pm*v.x() + vm*q.x() + cm*(q.y()*v.z() - q.z()*v.y()),
                pm*v.y() + vm*q.y() + cm*(q.z()*v.x() - q.x()*v.z()),
                pm*v.z() + vm*q.z() + cm*(q.x()*v.y() - q.y()*v.x()));
    }

    template <typename T>
    quaternion<T> operator*(const quaternion<T>& p, T s) {
        return quaternion<T>(p.w()*s, p.x()*s, p.y()*s, p.z()*s);
    }

    template <typename T>
    quaternion<T> operator*(T s, const quaternion<T>& p) {
        return quaternion<T>(p.w()*s, p.x()*s, p.y()*s, p.z()*s);
    }

    template <typename T>
    quaternion<T> operator+(const quaternion<T>& p, const quaternion<T>& q) {
        return quaternion<T>(p.w()+q.w(), p.x()+q.x(), p.y()+q.y(), p.z()+q.z());
    }

    template <typename T>
    quaternion<T> operator-(const quaternion<T>& p, const quaternion<T>& q) {
        return quaternion<T>(p.w()-q.w(), p.x()-q.x(), p.y()-q.y(), p.z()-q.z());
    }

    template <typename T>
    quaternion<T> slerp(T t, const quaternion<T>& start, const quaternion<T>& end) {
        T cos_theta = start.dot(end);
        T s_interp;
        T e_interp;

        if(cos_theta >= std::numeric_limits<float>::epsilon()) {
            if(((T)1 - cos_theta) > std::numeric_limits<float>::epsilon()) {
                T theta = acos(cos_theta);
                T inv_sin_theta = (T)1/sin(theta);

                s_interp = sin(((T)1 - t)*theta)*inv_sin_theta;
                e_interp = sin(t*theta)*inv_sin_theta;
            } else {
                s_interp = (T)1 - t;
                e_interp = t;
            }
        } else {
            if(((T)1 + cos_theta) > std::numeric_limits<float>::epsilon()) {
                T theta = acos(-cos_theta);
                T inv_sin_theta = (T)1/sin(theta);

                s_interp = sin((t-(T)1)*theta)*inv_sin_theta;
                e_interp = sin(t*theta)*inv_sin_theta;
            } else {
                s_interp = t - (T)1;
                e_interp = t;
            }
        }

        return s_interp*start + e_interp*end;
    }
}}

#endif //ZAP_QUATERNION_HPP
