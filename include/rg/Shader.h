//
// Created by matf-rg on 30.10.20.
//

#ifndef CG_PROJECT_SHADER_H
#define CG_PROJECT_SHADER_H

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include <glad/glad.h>
#include <glm/glm.hpp>

// #include <rg/Error.h>
// #include <common.h>

namespace rg {

    class Shader {
    private:
        unsigned int m_Id;
    public:
        Shader(std::string vertexShaderPath, std::string fragmentShaderPath);

        ~Shader();
        // activate the shader
        void use();

        // utility uniform functions
        void setBool(const std::string &name, bool value) const;

        void setInt(const std::string &name, int value) const;

        void setFloat(const std::string &name, float value) const;

        void setVec2(const std::string &name, const glm::vec2 &value) const;

        void setVec2(const std::string &name, float x, float y) const;

        void setVec3(const std::string &name, const glm::vec3 &value) const;

        void setVec3(const std::string &name, float x, float y, float z) const;

        void setVec4(const std::string &name, const glm::vec4 &value) const;

        void setVec4(const std::string &name, float x, float y, float z, float w);

        void setMat2(const std::string &name, const glm::mat2 &mat) const;

        void setMat3(const std::string &name, const glm::mat3 &mat) const;

        void setMat4(const std::string &name, const glm::mat4 &mat) const;

        void deleteProgram();
    };
}

#endif //CG_PROJECT_SHADER_H