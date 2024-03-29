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
        unsigned int EBO;
        bool normalMapping;

        std::vector<float> makeVertexMatrix(std::vector<float> &pos, std::vector<float> &tex);
        std::pair<glm::vec3, glm::vec3> calcTanBitanVec(glm::vec3 edge1, glm::vec3 edge2, glm::vec2 deltaUV1, glm::vec2 deltaUV2);
        void setupHexagon1();
        void setupHexagon2();

    public:
        std::vector<float> vertices;
        std::vector<unsigned int> indices;

        Hexagon(std::vector<float> &pos, std::vector<float> &tex, bool ind);
        void drawHexagon();
        void free();
    };

}

#endif //CG_PROJECT_HEXAGON_H