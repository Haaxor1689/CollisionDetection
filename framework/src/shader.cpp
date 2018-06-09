#include "shader.hpp"
#include "utility.hpp"

#include <memory>
#include <vector>

ShaderCompilationStatus Shader::GetCompilationStatus() const {
    GLint compileStatus;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compileStatus);

    return compileStatus == GL_TRUE ? ShaderCompilationStatus::Success : ShaderCompilationStatus::Failure;
}

std::string Shader::GetLog() const {
    // Retrieve length of the information log
    auto logLen = 0;
    glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logLen);

    // Allocate
    std::vector<char> log(logLen);
    glGetShaderInfoLog(shaderId, logLen, nullptr, log.data());

    return std::string(begin(log), end(log));
}

VertexShader::VertexShader(const std::string& fileName)
    : Shader(fileName) {
    // Load file into string
    const auto source = Utility::LoadFileToString(fileName);

    // Generate new ID
    shaderId = glCreateShader(GL_VERTEX_SHADER);

    // Load source into OpenGL driver
    auto sources = source.c_str();
    glShaderSource(shaderId, 1, &sources, nullptr);

    // Ask for compilation
    glCompileShader(shaderId);
}

FragmentShader::FragmentShader(const std::string& fileName)
    : Shader(fileName) {
    // Load file into strings
    const auto source = Utility::LoadFileToString(fileName);

    // Generate new ID
    shaderId = glCreateShader(GL_FRAGMENT_SHADER);

    // Load source into OpenGL driver
    auto sources = source.c_str();
    glShaderSource(shaderId, 1, &sources, nullptr);

    // Ask for compilation
    glCompileShader(shaderId);
}