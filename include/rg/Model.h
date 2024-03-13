//
// Created by matf-rg on 4.12.20.
//

#ifndef CG_PROJECT_MODEL_H
#define CG_PROJECT_MODEL_H
#include <stb_image.h>
#include <vector>
#include <string>

#include "rg/Shader.h"
#include "rg/Mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
//#include <rg/Error.h>

namespace rg {

    class Model {
    public:
        std::vector<Mesh> meshes;
        std::vector<Texture> loaded_textures;
        std::string directory;

        Model(std::string path);

        void Draw(Shader &shader);

    private:
        void loadModel(std::string path);

        void processNode(aiNode *node, const aiScene *scene);

        Mesh processMesh(aiMesh *mesh, const aiScene *scene);

        void
        loadTextureMaterial(aiMaterial *mat, aiTextureType type, std::string typeName, std::vector<Texture> &textures);
    };

    unsigned int TextureFromFile(const char *filename, std::string directory);
}

#endif //CG_PROJECT_MODEL_H