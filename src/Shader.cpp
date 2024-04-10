//
// Created by ana on 3.3.24.
//

#include "rg/Shader.h"
#include "rg/Error.h"
#include "common.h"

namespace rg {

    Shader::Shader(std::string vertexShaderPath, std::string fragmentShaderPath) {
        // appendShaderFolderIfNotPresent(vertexShaderPath);
        // appendShaderFolderIfNotPresent(fragmentShaderPath);
        // build and compile shader program

        // vertex shader
        std::string vsString = readFileContents(vertexShaderPath);
        ASSERT(!vsString.empty(), "Vertex shader source is empty!");
        const char *vertexShaderSource = vsString.c_str();

        unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
        glCompileShader(vertexShader);
        // check for shader compile errors
        int success;
        char infoLog[512];
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        // fragment shader
        std::string fsString = readFileContents(fragmentShaderPath);
        ASSERT(!fsString.empty(), "Fragment shader empty!");
        const char *fragmentShaderSource = fsString.c_str();

        unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
        glCompileShader(fragmentShader);
        // check for shader compile errors
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        // link shaders
        unsigned int shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);
        // check for linking errors
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        m_Id = shaderProgram;
    }

    // activate the shader
    void Shader::use() {
        glUseProgram(m_Id);
    }

    // utility uniform functions
    void Shader::setBool(const std::string &name, bool value) const {
        glUniform1i(glGetUniformLocation(m_Id, name.c_str()), (int) value);
    }

    void Shader::setInt(const std::string &name, int value) const {
        glUniform1i(glGetUniformLocation(m_Id, name.c_str()), value);
    }

    void Shader::setFloat(const std::string &name, float value) const {
        glUniform1f(glGetUniformLocation(m_Id, name.c_str()), value);
    }

    void Shader::setVec2(const std::string &name, const glm::vec2 &value) const {
        glUniform2fv(glGetUniformLocation(m_Id, name.c_str()), 1, &value[0]);
    }

    void Shader::setVec2(const std::string &name, float x, float y) const {
        glUniform2f(glGetUniformLocation(m_Id, name.c_str()), x, y);
    }

    void Shader::setVec3(const std::string &name, const glm::vec3 &value) const {
        glUniform3fv(glGetUniformLocation(m_Id, name.c_str()), 1, &value[0]);
    }

    void Shader::setVec3(const std::string &name, float x, float y, float z) const {
        glUniform3f(glGetUniformLocation(m_Id, name.c_str()), x, y, z);
    }

    void Shader::setVec4(const std::string &name, const glm::vec4 &value) const {
        glUniform4fv(glGetUniformLocation(m_Id, name.c_str()), 1, &value[0]);
    }

    void Shader::setVec4(const std::string &name, float x, float y, float z, float w) {
        glUniform4f(glGetUniformLocation(m_Id, name.c_str()), x, y, z, w);
    }

    void Shader::setMat2(const std::string &name, const glm::mat2 &mat) const {
        glUniformMatrix2fv(glGetUniformLocation(m_Id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void Shader::setMat3(const std::string &name, const glm::mat3 &mat) const {
        glUniformMatrix3fv(glGetUniformLocation(m_Id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const {
        glUniformMatrix4fv(glGetUniformLocation(m_Id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    Shader::~Shader() {
        deleteProgram();
    }

    void Shader::deleteProgram() {
        glDeleteProgram(m_Id);
        m_Id = 0;
    }

}