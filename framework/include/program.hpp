#pragma once

#include <glad/glad.h>

#include <memory>
#include <string>
#include <vector>

#include "shader.hpp"

class ShaderProgram {
public:
    ShaderProgram() = delete;
    ShaderProgram(const std::string& vertexFilename, const std::string& fragmentFilename);
    ShaderProgram(const std::string& vertexFilename, const std::string& fragmentFilename, std::vector<GLuint> positions, std::vector<std::string> names);

    GLint GetAttributeLocation(const std::string& attributeName) const;
    GLint GetUniformLocation(const std::string& uniformName) const;

    void Use() const;

    ~ShaderProgram();

private:
    GLuint programId = 0;
    std::unique_ptr<VertexShader> vertexShader;
    std::unique_ptr<FragmentShader> fragmentShader;
};
