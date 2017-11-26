//
// Created by Darren Otgaar on 2016/06/04.
//

#ifndef ZAP_GEOMETRY2_HPP
#define ZAP_GEOMETRY2_HPP

#include <graphics/graphics2/g2_types.hpp>
#include <graphics/generators/geometry/geometry_traits.hpp>

namespace zap { namespace generators {
    template <typename VertexT, primitive_type Primitive> struct geometry2;

    template <typename VertexT>
    struct geometry2<VertexT, primitive_type::PT_TRIANGLE_FAN> : public geometry_traits<VertexT, primitive_type::PT_TRIANGLE_FAN> {
        template <typename T> using vec2 = zap::maths::vec2<T>;
        using vec2f = zap::maths::vec2f;

        template <typename T>
        static std::vector<VertexT> make_quad(const vec2<T>& dims=vec2f{1.f, 1.f});

        template <typename T>
        static std::vector<VertexT> make_disc(size_t slices, T radius=1.f, const vec2<T>& C=vec2f{0.f, 0.f});
    };

    template <typename VertexT>
    template <typename T>
    std::vector<VertexT>
    geometry2<VertexT, primitive_type::PT_TRIANGLE_FAN>::make_quad(const vec2<T>& dims) {
        const auto hdim = T(.5)*dims;
        std::vector<VertexT> vertices(4);

        const auto position = VertexT::find(engine::attribute_type::AT_POSITION);
        const auto texcoord = VertexT::find(engine::attribute_type::AT_TEXCOORD1);

        if(position != INVALID_IDX) {
            vertices[0].set(position, vec2<T>{-hdim.x, -hdim.y});
            vertices[1].set(position, vec2<T>{+hdim.x, -hdim.y});
            vertices[2].set(position, vec2<T>{+hdim.x, +hdim.y});
            vertices[3].set(position, vec2<T>{-hdim.x, +hdim.y});
        }

        if(texcoord != INVALID_IDX) {
            vertices[0].set(texcoord, vec2<T>{T(0.), T(0.)});
            vertices[1].set(texcoord, vec2<T>{T(1.), T(0.)});
            vertices[2].set(texcoord, vec2<T>{T(1.), T(1.)});
            vertices[3].set(texcoord, vec2<T>{T(0.), T(1.)});
        }

        return vertices;
    }

    template <typename VertexT>
    template <typename T>
    std::vector<VertexT>
    geometry2<VertexT, primitive_type::PT_TRIANGLE_FAN>::make_disc(size_t slices, T radius, const vec2<T>& C) {
        // Assume either LINE_LOOP or TRI_FAN (intended to be specialised if generic cost is too great)
        auto vertex_count = slices+2;
        std::vector<VertexT> vertices(vertex_count);
        const T dt = maths::TWO_PI<T>/slices;
        const auto end = slices+1;
        const size_t texcoord = VertexT::find(engine::attribute_type::AT_TEXCOORD1);

        size_t idx = 0;
        vertices[idx].position = C;
        if(texcoord != INVALID_IDX) vertices[idx].set(texcoord, vec2<T>(T(.5), T(.5)));
        idx++;

        T theta = 0;
        for(size_t i = 0; i != end; ++i) {
            theta = (i % slices) * dt;
            T stheta = std::sin(theta), ctheta = std::cos(theta);
            vertices[idx].position = C + vec2<T>(radius * ctheta, radius * stheta);
            if(texcoord != INVALID_IDX) vertices[idx].set(texcoord, vec2<T>(T(.5) + T(.5)*ctheta, T(.5) + T(.5)*stheta));
            idx++;
        }

        return vertices;
    }

    template <typename VertexT>
    struct geometry2<VertexT, primitive_type::PT_LINE_LOOP> : public geometry_traits<VertexT, primitive_type::PT_LINE_LOOP> {
        template <typename T> using vec2 = zap::maths::vec2<T>;
        using vec2f = zap::maths::vec2f;

        template <typename T>
        static std::vector<VertexT> make_disc(const vec2<T>& C, T radius, size_t slices);
    };

    template <typename VertexT>
    template <typename T>
    std::vector<VertexT>
    geometry2<VertexT, primitive_type::PT_LINE_LOOP>::make_disc(const vec2<T>& C, T radius, size_t slices) {
        // Assume either LINE_LOOP or TRI_FAN (intended to be specialised if generic cost is too great)
        auto vertex_count = slices;
        std::vector<VertexT> vertices(vertex_count);
        const T dt = maths::TWO_PI<T>/slices;
        const auto end = slices;
        const size_t texcoord = VertexT::find(engine::attribute_type::AT_TEXCOORD1);

        size_t idx = 0;
        T theta = 0;
        for(size_t i = 0; i != end; ++i) {
            theta = (i % slices) * dt;
            T stheta = std::sin(theta), ctheta = std::cos(theta);
            vertices[idx].position = vec2<T>(radius * ctheta, radius * stheta);
            if(texcoord != INVALID_IDX) vertices[idx].set(texcoord, vec2<T>(T(.5) + T(.5)*ctheta, T(.5) + T(.5)*stheta));
            idx++;
        }

        return vertices;
    }

    template <typename VertexT>
    struct geometry2<VertexT, primitive_type::PT_TRIANGLE_STRIP> : public geometry_traits<VertexT, primitive_type::PT_TRIANGLE_STRIP> {
        template <typename T> using vec2 = zap::maths::vec2<T>;
        using vec2f = zap::maths::vec2f;

        template <typename T>
        static std::vector<VertexT> make_quad(const vec2<T>& dims=vec2f{1.f, 1.f});
    };

    template <typename VertexT>
    template <typename T>
    std::vector<VertexT>
    geometry2<VertexT, primitive_type::PT_TRIANGLE_STRIP>::make_quad(const vec2<T>& dims) {
        const auto hdim = T(.5)*dims;
        std::vector<VertexT> vertices(4);

        const auto position = VertexT::find(engine::attribute_type::AT_POSITION);
        const auto texcoord = VertexT::find(engine::attribute_type::AT_TEXCOORD1);

        if(position != INVALID_IDX) {
            vertices[0].set(position, vec2<T>{-hdim.x, +hdim.y});
            vertices[1].set(position, vec2<T>{-hdim.x, -hdim.y});
            vertices[2].set(position, vec2<T>{+hdim.x, +hdim.y});
            vertices[3].set(position, vec2<T>{+hdim.x, -hdim.y});
        }

        if(texcoord != INVALID_IDX) {
            vertices[0].set(texcoord, vec2<T>{T(0.), T(1.)});
            vertices[1].set(texcoord, vec2<T>{T(0.), T(0.)});
            vertices[2].set(texcoord, vec2<T>{T(1.), T(1.)});
            vertices[3].set(texcoord, vec2<T>{T(1.), T(0.)});
        }

        return vertices;
    }

}}

#endif //ZAP_GEOMETRY2_HPP
