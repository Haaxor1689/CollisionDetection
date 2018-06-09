#include "program.hpp"

#include <vector>

ShaderProgram::ShaderProgram(const std::string& vertexFilename, const std::string& fragmentFilename) {
    vertexShader = std::make_unique<VertexShader>(vertexFilename);
    if (vertexShader->GetCompilationStatus() == ShaderCompilationStatus::Failure) {
        throw std::runtime_error(vertexShader->GetLog());
    }

    fragmentShader = std::make_unique<FragmentShader>(fragmentFilename);
    if (fragmentShader->GetCompilationStatus() == ShaderCompilationStatus::Failure) {
        throw std::runtime_error(fragmentShader->GetLog());
    }

    programId = glCreateProgram();
    glAttachShader(programId, vertexShader->GetId());
    glAttachShader(programId, fragmentShader->GetId());
    glLinkProgram(programId);

    auto compileStatus = GL_FALSE;
    glGetProgramiv(programId, GL_LINK_STATUS, &compileStatus);
    if (compileStatus == GL_FALSE) {
        // Retrieve length of the information log
        auto logLen = 0;
        glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &logLen);

        // Allocate
        std::vector<char> log(logLen);
        glGetProgramInfoLog(programId, logLen, nullptr, log.data());

        throw std::runtime_error(std::string(begin(log), end(log)));
    }
}

ShaderProgram::ShaderProgram(const std::string& vertexFilename, const std::string& fragmentFilename, std::vector<GLuint> positions, std::vector<std::string> names) {
    vertexShader = std::make_unique<VertexShader>(vertexFilename);
    if (vertexShader->GetCompilationStatus() == ShaderCompilationStatus::Failure) {
        const auto log = vertexShader->GetLog();
    }

    fragmentShader = std::make_unique<FragmentShader>(fragmentFilename);
    if (fragmentShader->GetCompilationStatus() == ShaderCompilationStatus::Failure) {
        const auto log = fragmentShader->GetLog();
    }

    programId = glCreateProgram();
    glAttachShader(programId, vertexShader->GetId());
    glAttachShader(programId, fragmentShader->GetId());

    for (uint32_t i = 0; i < positions.size(); i++) {
        glBindAttribLocation(programId, positions[i], names[i].c_str());
    }

    glLinkProgram(programId);

    auto compileStatus = GL_FALSE;
    glGetProgramiv(programId, GL_LINK_STATUS, &compileStatus);
    if (compileStatus == GL_FALSE) {
        // Retrieve length of the information log
        auto logLen = 0;
        glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &logLen);

        // Allocate
        std::vector<char> log(logLen);
        glGetProgramInfoLog(programId, logLen, nullptr, log.data());

        throw std::runtime_error(std::string(begin(log), end(log)));
    }
}

GLint ShaderProgram::GetAttributeLocation(const std::string& attributeName) const {
    return glGetAttribLocation(programId, attributeName.c_str());
}

GLint ShaderProgram::GetUniformLocation(const std::string& uniformName) const {
    return glGetUniformLocation(programId, uniformName.c_str());
}

void ShaderProgram::Use() const {
    glUseProgram(programId);
}

ShaderProgram::~ShaderProgram() {
    glDeleteProgram(programId);
}
