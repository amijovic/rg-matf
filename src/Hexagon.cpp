//
// Created by ana on 3.3.24.
//

#include <glad/glad.h>
#include "rg/Hexagon.h"
#include <iostream>

namespace rg {

    Hexagon::Hexagon(std::vector<float> &pos, std::vector<float> &tex, bool ind) {
        normalMapping = ind;
        if(normalMapping) {
            vertices = makeVertexMatrix(pos, tex);
            setupHexagon1();
        }
        else {
            vertices = {
                    pos[0], pos[1], pos[2], 0.0f, 0.1f, 0.0f, tex[0], tex[1],
                    pos[3], pos[4], pos[5], 0.0f, 0.1f, 0.0f, tex[2], tex[3],
                    pos[6], pos[7], pos[8], 0.0f, 0.1f, 0.0f, tex[4], tex[5],
                    pos[9], pos[10], pos[11], 0.0f, 0.1f, 0.0f, tex[6], tex[7],
                    pos[12], pos[13], pos[14], 0.0f, 0.1f, 0.0f, tex[8], tex[9],
                    pos[15], pos[16], pos[17], 0.0f, 0.1f, 0.0f, tex[10], tex[11],
                    pos[18], pos[19], pos[20], 0.0f, 0.1f, 0.0f, tex[12], tex[13]
            };
            indices = {
                    0, 1, 2,    // first triangle
                    0, 2, 3,    // second triangle
                    0, 3, 4,
                    0, 4, 5,
                    0, 5, 6,
                    0, 6, 1
            };
            setupHexagon2();
        }
        glBindVertexArray(0);
    }

    std::vector<float> Hexagon::makeVertexMatrix(std::vector<float> &pos, std::vector<float> &tex) {
        // normal vector
        glm::vec3 nm(0.0f, 0.0f, 1.0f);
        std::pair<glm::vec3, glm::vec3> p;

        // triangle 012
        glm::vec3 tangent1, bitangent1;
        p = calcTanBitanVec(glm::vec3(pos[3] - pos[0], pos[4] - pos[1], pos[5] - pos[2]),
                            glm::vec3(pos[6] - pos[0], pos[7] - pos[1], pos[8] - pos[2]),
                            glm::vec2(tex[2] - tex[0], tex[3] - tex[1]),
                            glm::vec2(tex[4] - tex[0], tex[5] - tex[1]));
        tangent1 = p.first;
        bitangent1 = p.second;
        // triangle 023
        glm::vec3 tangent2, bitangent2;
        p = calcTanBitanVec(glm::vec3(pos[6] - pos[0], pos[7] - pos[1], pos[8] - pos[2]),
                            glm::vec3(pos[9] - pos[0], pos[10] - pos[1], pos[11] - pos[2]),
                            glm::vec2(tex[4] - tex[0], tex[5] - tex[1]),
                            glm::vec2(tex[6] - tex[0], tex[7] - tex[1]));
        tangent2 = p.first;
        bitangent2 = p.second;
        // triangle 034
        glm::vec3 tangent3, bitangent3;
        p = calcTanBitanVec(glm::vec3(pos[9] - pos[0], pos[10] - pos[1], pos[11] - pos[2]),
                            glm::vec3(pos[12] - pos[0], pos[13] - pos[1], pos[14] - pos[2]),
                            glm::vec2(tex[6] - tex[0], tex[7] - tex[1]),
                            glm::vec2(tex[8] - tex[0], tex[9] - tex[1]));
        tangent3 = p.first;
        bitangent3 = p.second;
        // triangle 045
        glm::vec3 tangent4, bitangent4;
        p = calcTanBitanVec(glm::vec3(pos[12] - pos[0], pos[13] - pos[1], pos[14] - pos[2]),
                            glm::vec3(pos[15] - pos[0], pos[16] - pos[1], pos[17] - pos[2]),
                            glm::vec2(tex[8] - tex[0], tex[9] - tex[1]),
                            glm::vec2(tex[10] - tex[0], tex[11] - tex[1]));
        tangent4 = p.first;
        bitangent4 = p.second;
        // triangle 056
        glm::vec3 tangent5, bitangent5;
        p = calcTanBitanVec(glm::vec3(pos[15] - pos[0], pos[16] - pos[1], pos[17] - pos[2]),
                            glm::vec3(pos[18] - pos[0], pos[19] - pos[1], pos[20] - pos[2]),
                            glm::vec2(tex[10] - tex[0], tex[11] - tex[1]),
                            glm::vec2(tex[12] - tex[0], tex[13] - tex[1]));
        tangent5 = p.first;
        bitangent5 = p.second;
        // triangle 061
        glm::vec3 tangent6, bitangent6;
        p = calcTanBitanVec(glm::vec3(pos[18] - pos[0], pos[19] - pos[1], pos[20] - pos[2]),
                            glm::vec3(pos[3] - pos[0], pos[4] - pos[1], pos[5] - pos[2]),
                            glm::vec2(tex[12] - tex[0], tex[13] - tex[1]),
                            glm::vec2(tex[2] - tex[0], tex[3] - tex[1]));
        tangent6 = p.first;
        bitangent6 = p.second;


        std::vector<float> quadVertices = {
                // positions            // normal         // texcoords    // tangent                          // bitangent
                // triangle 012
                pos[0], pos[1], pos[2], nm.x, nm.y, nm.z, tex[0], tex[1], tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
                pos[3], pos[4], pos[5], nm.x, nm.y, nm.z, tex[2], tex[3], tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
                pos[6], pos[7], pos[8], nm.x, nm.y, nm.z, tex[4], tex[5], tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
                // triangle 023
                pos[0], pos[1], pos[2], nm.x, nm.y, nm.z, tex[0], tex[1], tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
                pos[6], pos[7], pos[8], nm.x, nm.y, nm.z, tex[4], tex[5], tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
                pos[9], pos[10], pos[11], nm.x, nm.y, nm.z, tex[6], tex[7], tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
                // triangle 034
                pos[0], pos[1], pos[2], nm.x, nm.y, nm.z, tex[0], tex[1], tangent3.x, tangent3.y, tangent3.z, bitangent3.x, bitangent3.y, bitangent3.z,
                pos[9], pos[10], pos[11], nm.x, nm.y, nm.z, tex[6], tex[7], tangent3.x, tangent3.y, tangent3.z, bitangent3.x, bitangent3.y, bitangent3.z,
                pos[12], pos[13], pos[14], nm.x, nm.y, nm.z, tex[8], tex[9], tangent3.x, tangent3.y, tangent3.z, bitangent3.x, bitangent3.y, bitangent3.z,
                // triangle 045
                pos[0], pos[1], pos[2], nm.x, nm.y, nm.z, tex[0], tex[1], tangent4.x, tangent4.y, tangent4.z, bitangent4.x, bitangent4.y, bitangent4.z,
                pos[12], pos[13], pos[14], nm.x, nm.y, nm.z, tex[8], tex[9], tangent4.x, tangent4.y, tangent4.z, bitangent4.x, bitangent4.y, bitangent4.z,
                pos[15], pos[16], pos[17], nm.x, nm.y, nm.z, tex[10], tex[11], tangent4.x, tangent4.y, tangent4.z, bitangent4.x, bitangent4.y, bitangent4.z,
                // triangle 056
                pos[0], pos[1], pos[2], nm.x, nm.y, nm.z, tex[0], tex[1], tangent5.x, tangent5.y, tangent5.z, bitangent5.x, bitangent5.y, bitangent5.z,
                pos[15], pos[16], pos[17], nm.x, nm.y, nm.z, tex[10], tex[11], tangent5.x, tangent5.y, tangent5.z, bitangent5.x, bitangent5.y, bitangent5.z,
                pos[18], pos[19], pos[20], nm.x, nm.y, nm.z, tex[12], tex[13], tangent5.x, tangent5.y, tangent5.z, bitangent5.x, bitangent5.y, bitangent5.z,
                // triangle 061
                pos[0], pos[1], pos[2], nm.x, nm.y, nm.z, tex[0], tex[1], tangent6.x, tangent6.y, tangent6.z, bitangent6.x, bitangent6.y, bitangent6.z,
                pos[18], pos[19], pos[20], nm.x, nm.y, nm.z, tex[12], tex[13], tangent6.x, tangent6.y, tangent6.z, bitangent6.x, bitangent6.y, bitangent6.z,
                pos[3], pos[4], pos[5], nm.x, nm.y, nm.z, tex[2], tex[3], tangent6.x, tangent6.y, tangent6.z, bitangent6.x, bitangent6.y, bitangent6.z
        };

        return quadVertices;
    }

    std::pair<glm::vec3, glm::vec3> Hexagon::calcTanBitanVec(glm::vec3 edge1, glm::vec3 edge2, glm::vec2 deltaUV1, glm::vec2 deltaUV2) {
        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
        glm::vec3 tangent, bitangent;

        tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

        bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

        return std::make_pair(tangent, bitangent);
    }

    void Hexagon::setupHexagon1() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO); // activate VAO

        glBindBuffer(GL_ARRAY_BUFFER, VBO); // activate buffer
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0],GL_STATIC_DRAW); // copy user defined data into the current bind buffer

        // position attribute
        glEnableVertexAttribArray(0); // activate defined vao
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void *)0);
        // normals attribute
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void *) (3 * sizeof(float)));
        // texture coord attribute
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void *) (6 * sizeof(float)));
        // tangent attribute
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void *) (8 * sizeof(float)));
        // bitangent attribute
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void *) (11 * sizeof(float)));
    }

    void Hexagon::setupHexagon2() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0],GL_STATIC_DRAW); // copy user defined data into the current bind buffer

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(float), &indices[0], GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0); // activate defined vao
        // normals attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        // texture coord attribute
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (6 * sizeof(float)));
        glEnableVertexAttribArray(2);
    }

    void Hexagon::drawHexagon() {
        glBindVertexArray(VAO);
        if (normalMapping) {
            glDrawArrays(GL_TRIANGLES, 0, 6 * 3); // render triangles
        }
        else {
            glDrawElements(GL_TRIANGLES, 3 * 6, GL_UNSIGNED_INT, 0);
        }
        glBindVertexArray(0);
    }

    void Hexagon::free() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        if (!normalMapping) {
            glDeleteBuffers(1, &EBO);
        }
    }

}