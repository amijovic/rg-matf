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

        int width, height, nrChannels;
        unsigned char *data = stbi_load(FileSystem::getPath(path).c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            GLenum format;
            if (nrChannels == 1) {
                format = GL_RED;
            }
            else if (nrChannels == 3) {
                format = GL_RGB;
            }
            else if (nrChannels == 4) {
                format = GL_RGBA;
            }

            glBindTexture(GL_TEXTURE_2D, t);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);
        } else {
            std::cout << "Failed to load texture" << std::endl;
            stbi_image_free(data);
        }

        return t;
    }

    unsigned int Texture2D::getId() {
        return texture;
    }

}