/* Created by Darren Otgaar on 2016/09/18. http://www.github.com/otgaard/zap */
#ifndef ZAP_LIGHT_HPP
#define ZAP_LIGHT_HPP

#include <maths/algebra.hpp>
#include <engine/uniform_buffer.hpp>

namespace zap { namespace renderer {
    using light_dir_basic = engine::uniform_block<
        core::light_dir<maths::vec4f>,
        core::light_colour<maths::vec4f>,
        core::light_intensity<float>
    >;

    using light_pos_basic = engine::uniform_block<
        core::light_position<maths::vec4f>,
        core::light_attenuation<maths::vec4f>,
        core::light_colour<maths::vec3f>,
        core::light_intensity<float>
    >;

    template <size_t N>
    using lights_dir = engine::uniform_block<
        core::lights_dir<std::array<light_dir_basic, N>>,
        core::light_count<int>
    >;

    template<size_t N>
    using lights_pos = engine::uniform_block<
        core::lights_point<std::array<light_pos_basic, N>>,
        core::light_count<int>
    >;

    struct light_base {

    };

    template <typename BlockT>
    struct light : public light_base {
        using block_t = BlockT;
        block_t block;
    };

    using dir_light = light<light_dir_basic>;

}}

#endif //ZAP_LIGHT_HPP
