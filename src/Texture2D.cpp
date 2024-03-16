//
// Created by ana on 29.2.24.
//

#include <glad/glad.h>
#include "rg/Texture2D.h"
#include "learnopengl/filesystem.h"
#include <iostream>

namespace rg {

    Texture2D::Texture2D(std::string path) {
        texture = loadTexture(path);
    }

    unsigned int Texture2D::loadTexture(std::string path) {
        unsigned int t;
        glGenTextures(1, &t);
        glBindTexture(GL_TEXTURE_2D, t);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(true);
        unsigned char *data = stbi_load(FileSystem::getPath(path).c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            std::cout << "Failed to load texture" << std::endl;
        }
        stbi_image_free(data);
        return t;
    }

    unsigned int Texture2D::getId() {
        return texture;
    }

}