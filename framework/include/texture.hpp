#pragma once

#include <glad/glad.h>
#include <string>

namespace Texture {

const GLenum cubemapSides[6] = {
    GL_TEXTURE_CUBE_MAP_POSITIVE_X,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
};

GLuint LoadTexture2D(const std::string& filename);
GLuint LoadTextureCubemap(std::string filenames[6]);

}
