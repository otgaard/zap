/* Created by Darren Otgaar on 2016/09/18. http://www.github.com/otgaard/zap */
#ifndef ZAP_LIGHT_HPP
#define ZAP_LIGHT_HPP

#include <maths/algebra.hpp>
#include <engine/uniform_buffer.hpp>

namespace zap { namespace renderer {
    using light_dir_basic = engine::uniform_block<
        core::light_dir<maths::vec4f>,
        core::light_intensity<float>,
        core::light_exponent<float>
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
