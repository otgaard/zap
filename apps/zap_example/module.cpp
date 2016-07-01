/* Created by Darren Otgaar on 2016/06/30. http://www.github.com/otgaard/zap */
#include "module.hpp"
#include <generators/noise/noise.hpp>

using namespace zap;
using namespace zap::engine;

//zap::engine::texture prn_texture;       // LUM8DN type (unsigned denormalised byte [0..255])
//zap::engine::texture g1g2_texture;      // RGB32F type (x = grad1, yz = grad2)
//zap::engine::texture g3_texture;        // RGB32F type (grad3)

noise_module::noise_module() : prn_texture(texture_type::TT_TEX1D), g1g2_texture(texture_type::TT_TEX1D), g3_texture(texture_type::TT_TEX1D) {
}

bool noise_module::initialise() {
    if(!generators::noise::is_initialised()) generators::noise::initialise();

    LOG("Step 0");
    if(!prn_texture.allocate()) { LOG_ERR("Couldn't initialise noise PRN table texture"); return false; }
    prn_texture.initialise(texture_type::TT_TEX1D, 256, 1, pixel_format::PF_RED, pixel_datatype::PD_DN_UNSIGNED_BYTE, generators::noise::prn_tbl_ptr());

    if(!g1g2_texture.allocate()) { LOG_ERR("Couldn't initialise Grad1 or Grad2 texture"); return false; }

    // Copy g1 and g2 to shared buffer
    float g1g2[generators::noise::prn_tbl_size*3];
    for(int i = 0; i != generators::noise::prn_tbl_size; ++i) {
        auto idx = 3*i;
        g1g2[idx] = *(generators::noise::grad1_tbl_ptr()+i);
        const auto& v = *(generators::noise::grad2_tbl_ptr()+i);
        g1g2[idx+1] = v.x; g1g2[idx+2] = v.y;
    }

    LOG("Step 1");
    g1g2_texture.initialise(texture_type::TT_TEX1D, 256, 1, pixel_format::PF_RGB, pixel_datatype::PD_FLOAT, g1g2);

    LOG("Step 2");
    if(!g3_texture.allocate()) { LOG_ERR("Couldn't initialise Grad3 texture"); return false; }
    g3_texture.initialise(texture_type::TT_TEX1D, 256, 1, pixel_format::PF_RGB, pixel_datatype::PD_FLOAT, generators::noise::grad3_tbl_ptr()->data());

    return !gl_error_check();
}

void noise_module::bind(int u0, int u1, int u2) {
    prn_texture.bind(0);
    //g1g2_texture.bind(0);
    //g3_texture.bind(u2);
}

void noise_module::release() {
    prn_texture.release();
    //g1g2_texture.release();
    //g3_texture.release();
}
