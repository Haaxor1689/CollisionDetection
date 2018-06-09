#pragma once

#include <glad/glad.h>

#include <string>

enum class ShaderCompilationStatus {
    Success,
    Failure
};

class Shader {
public:
    Shader() = delete;
    Shader(const std::string& fileName) {}
    virtual ~Shader() { glDeleteShader(this->shaderId); }

    ShaderCompilationStatus GetCompilationStatus() const;
    std::string GetLog() const;
    GLuint GetId() const { return shaderId; }

protected:
    GLuint shaderId = 0;
};

class VertexShader : public Shader {
public:
    VertexShader() = delete;
    VertexShader(const std::string& fileName);
};

class FragmentShader : public Shader {
public:
    FragmentShader() = delete;
    FragmentShader(const std::string& fileName);
};
