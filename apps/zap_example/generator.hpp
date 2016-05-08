/* Created by Darren Otgaar on 2016/05/08. http://www.github.com/otgaard/zap */
#ifndef ZAP_GENERATOR_HPP
#define ZAP_GENERATOR_HPP

#include <engine/texture.hpp>
#include <engine/pixel_format.hpp>

// Mesh, Texture, and Noise Generators

class generator {
public:
    static std::unique_ptr<zap::engine::texture> create_checker() {
        using namespace zap::engine;

        // This is a compressed texture (UNSIGNED_BYTE_3_3_2)
        constexpr size_t linc = 64/16;

        LOG(sizeof(rgb332_t)*64*64, sizeof(rgb332_t));

        std::vector<rgb332_t> pixbuf(64*64);
        for(size_t row = 0; row != 64; ++row) {
            for(size_t col = 0; col != 64; ++col) {
                auto& pixel = pixbuf[64*row+col];
                pixel.set(0, col/linc); pixel.set(1, row/linc);
                if(col < 32) {
                    if(row < 32) pixel.set(2, 0);
                    else         pixel.set(2, 1);
                } else {
                    if(row < 32) pixel.set(2, 2);
                    else         pixel.set(2, 3);
                }
                //pixel.set(0, col); pixel.set(1, row); pixel.set(2, col);
            }
        }

        auto ptr = std::make_unique<texture>();
        ptr->allocate();
        ptr->initialise(64, 64, pixbuf, false);
        return ptr;
    }
};

#endif //ZAP_GENERATOR_HPP
