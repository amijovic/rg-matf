//
// Created by matf-rg on 4.12.20.
//

#ifndef PROJECT_BASE_MESH_H
#define PROJECT_BASE_MESH_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
// #include <rg/Error.h>
#include <rg/Shader.h>

namespace rg {

    struct Vertex {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;

        glm::vec3 Tangent;
        glm::vec3 Bitangent;
    };

    struct Texture {
        unsigned int id;
        std::string type; // texture_diffuse, texture_specular, texture_normal, texture_height
        std::string path;
    };

    class Mesh {
    private:
        unsigned int VAO;
        void setupMesh();

    public:
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;

        Mesh(const std::vector<Vertex> &vs, const std::vector<unsigned int> &ind, const std::vector<Texture> &tex);
        void Draw(Shader &shader);
    };
}

#endif //PROJECT_BASE_MESH_H