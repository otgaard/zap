/* Created by Darren Otgaar on 2016/05/07. http://www.github.com/otgaard/zap */
#ifndef ZAP_TEXTURE_HPP
#define ZAP_TEXTURE_HPP

#include <vector>
#include "engine.hpp"
#include "pixel_format.hpp"

namespace zap { namespace engine {

class texture {
public:
    texture(texture_type type=texture_type::TT_TEX2D) : type_(type), id_(INVALID_RESOURCE) { }
    ~texture() { if(is_allocated()) deallocate(); }

    bool allocate();
    bool deallocate();
    bool is_allocated() const { return id_ != INVALID_RESOURCE; }

    resource_t resource() const { return id_; }

    void bind(size_t unit=0) const;
    void release() const;
    bool is_bound() const;

    // Test Function
    template <typename Pixel>
    bool initialise(size_t width, size_t height, const std::vector<Pixel>& buffer, bool generate_mipmaps=false);
    /*bool initialise(size_t width, size_t height, pixel_format format, pixel_datatype datatype);*/
    bool initialise(texture_type type, size_t width, size_t height, pixel_format format, pixel_datatype datatype,
                    const void* data=nullptr);

    static size_t query_max_units();

protected:
    texture_type type_;
    resource_t id_;
};

}}

#endif //ZAP_TEXTURE_HPP
