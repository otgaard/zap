/* Created by Darren Otgaar on 2016/06/30. http://www.github.com/otgaard/zap */
#ifndef ZAP_MODULE_HPP
#define ZAP_MODULE_HPP

#include <memory>
#include <engine/texture.hpp>

/*
 * A "module" is intended to represent the client and server resources necessary for some graphical resource.
 * The archetypal example is a noise module which renders noise to a texture for use in another module.  This module
 * is noise for now, so I can work through the interface, linkage, and rendering implications.
 */

class noise_module {
public:
    noise_module();

    bool initialise();
    void bind(int u0, int u1, int u2);
    void release();

private:
    zap::engine::texture prn_texture;       // LUM8DN type (unsigned denormalised byte [0..255]
    zap::engine::texture g1g2_texture;      // RGB32F type (x = grad1, yz = grad2)
    zap::engine::texture g3_texture;        // RGB32F type (grad3)
};

#endif //ZAP_MODULE_HPP
