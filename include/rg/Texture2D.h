//
// Created by ana on 3.3.24.
//

#ifndef CG_PROJECT_TEXTURE2D_H
#define CG_PROJECT_TEXTURE2D_H

#include <glad/glad.h>
#include <stb_image.h>
#include <iostream>

namespace rg {

    class Texture2D {
    private:
        unsigned int texture;
        unsigned int loadTexture(std::string path);

    public:
        Texture2D(std::string path);
        unsigned int getId();
    };

}

#endif //CG_PROJECT_TEXTURE2D_H