//
// Created by ana on 3.3.24.
//

#ifndef CG_PROJECT_HEXAGON_H
#define CG_PROJECT_HEXAGON_H

#include <vector>
#include "rg/Texture2D.h"
#include <GLFW/glfw3.h>

namespace rg {

    class Hexagon {
    private:
        unsigned int VAO;
        unsigned int VBO;
        unsigned int EBO;

        void setupHexagon();

    public:
        std::vector<float> vertices;
        std::vector<unsigned int> indices;
        Texture2D texture;

        Hexagon(std::vector<float> &vs, std::vector<unsigned int> &ind, Texture2D &tex);

        void bindTexture();

        void drawHexagon();

        void free();
    };

}

#endif //CG_PROJECT_HEXAGON_H