//
// Created by ana on 3.3.24.
//

#include "rg/Hexagon.h"

namespace rg {

    Hexagon::Hexagon(std::vector<float> &vs, std::vector<unsigned int> &ind, Texture2D &tex)
            : vertices(vs), indices(ind), texture(tex) {
        setupHexagon();
    }

    void Hexagon::setupHexagon() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO); // activate VAO

        glBindBuffer(GL_ARRAY_BUFFER, VBO); // activate buffer
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0],GL_STATIC_DRAW); // copy user defined data into the current bind buffer

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(float), &indices[0], GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0); // activate defined vao
        // normals attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (3 * sizeof(float)));
        glEnableVertexAttribArray(1); // activate defined vao
        // texture coord attribute
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (6 * sizeof(float)));
        glEnableVertexAttribArray(2); // activate defined vao
    }

    void Hexagon::bindTexture() {
        glBindVertexArray(VAO);
        glActiveTexture(GL_TEXTURE0);
       // texture.bindTexture();
    }

    void Hexagon::drawHexagon() {
        glDrawElements(GL_TRIANGLES, 3 * 6, GL_UNSIGNED_INT, 0); // render triangles from indexed buffer
        glBindVertexArray(0);
    }

    void Hexagon::free() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }

}