/* Created by Darren Otgaar on 2016/04/06. http://www.github.com/otgaard/zap */
#ifndef ZAP_VERTEX_BUFFER_HPP
#define ZAP_VERTEX_BUFFER_HPP

#include "buffer.hpp"
#include "buffer_format.hpp"

namespace zap { namespace engine {

template <typename VTX_T, buffer_usage USAGE>
class vertex_buffer : public buffer {
public:
    static_assert(is_specialisation_of<vertex, VTX_T>::value, "VTX_T must be a specialisation of vertex<>");

    using vertex_t = VTX_T;
    using pod_t = typename vertex_t::pod_t;
    constexpr static auto usage = USAGE;




};

}}

#endif //ZAP_VERTEX_BUFFER_HPP
