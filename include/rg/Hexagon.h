//
// Created by ana on 3.3.24.
//

#ifndef CG_PROJECT_HEXAGON_H
#define CG_PROJECT_HEXAGON_H

#include <vector>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

namespace rg {

    class Hexagon {
    private:
        unsigned int VAO;
        unsigned int VBO;

        std::vector<float> makeVertexMatrix(std::vector<float> &pos, std::vector<float> &tex);
        std::pair<glm::vec3, glm::vec3> calcTanBitanVec(glm::vec3 edge1, glm::vec3 edge2, glm::vec2 deltaUV1, glm::vec2 deltaUV2);
        void setupHexagon();

    public:
        std::vector<float> vertices;

        Hexagon(std::vector<float> &pos, std::vector<float> &tex);
        void drawHexagon();
        void free();
    };

}

#endif //CG_PROJECT_HEXAGON_H