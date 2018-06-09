#pragma once

#include <glad/glad.h>

#include <vector>
#include <string>

#include "Geometry"

class Mesh {
public:
    Mesh(const std::string& fileName, GLint positionLocation = -1, GLint normalLocation = -1,
         GLint texCoordLocation = -1);

    Mesh(std::vector<float> vertices, std::vector<uint32_t> indices, GLenum mode = GL_TRIANGLES,
         GLint positionLocation = -1, GLint normalLocation = -1, GLint texCoordLocation = -1)
        : Mesh(vertices, std::vector<float>(), std::vector<float>(), indices, mode, positionLocation, normalLocation,
               texCoordLocation) {}

    Mesh(std::vector<float> vertices, std::vector<float> normals, std::vector<uint32_t> indices,
         GLenum mode = GL_TRIANGLES, GLint positionLocation = -1, GLint normalLocation = -1,
         GLint texCoordLocation = -1)
        : Mesh(vertices, normals, std::vector<float>(), indices, mode, positionLocation, normalLocation,
               texCoordLocation) {}

    Mesh(std::vector<float> vertices, std::vector<float> normals, std::vector<float> texCoords,
         std::vector<uint32_t> indices, GLenum mode = GL_TRIANGLES, GLint positionLocation = -1,
         GLint normalLocation = -1, GLint texCoordLocation = -1);

    Mesh(const Mesh& other);

    void CreateVao(GLint positionLocation = -1, GLint normalLocation = -1, GLint texCoordLocation = -1);
    void SetTexture(GLuint textureId) { texture = textureId; }
    GLuint GetVaoId() const { return this->vaoId; }
    void BindVao() const { glBindVertexArray(this->vaoId); }

    void Draw() const;

    static Mesh FromInterleaved(std::vector<float> interleavedVertices, std::vector<uint32_t> indices,
                                 GLenum mode = GL_TRIANGLES, GLint positionLocation = -1, GLint normalLocation = -1,
                                 GLint texCoordLocation = -1);

    static std::vector<Mesh> FromFile(const std::string& fileName, GLint positionLocation = -1,
                                       GLint normalLocation = -1, GLint texCoordLocation = -1);

    static Mesh Cube(GLint positionLocation = -1, GLint normalLocation = -1, GLint texCoordLocation = -1);
    static Mesh Sphere(GLint positionLocation = -1, GLint normalLocation = -1, GLint texCoordLocation = -1);
    static Mesh Ground(GLint positionLocation = -1, GLint normalLocation = -1, GLint texCoordLocation = -1);
    static Mesh Pad(GLint positionLocation = -1, GLint normalLocation = -1, GLint texCoordLocation = -1);
    static Mesh Brick(GLint positionLocation = -1, GLint normalLocation = -1, GLint texCoordLocation = -1);

    ~Mesh();

private:
    GLuint vaoId = 0;

    size_t verticesCount = 0;
    GLuint verticesBufferId = 0;
    GLuint normalsBufferId = 0;
    GLuint texCoordsBufferId = 0;

    size_t indicesCount = 0;
    GLuint indicesBufferId = 0;

    GLint positionLocation = -1;
    GLint normalLocation = -1;
    GLint texCoordLocation = -1;

    GLenum mode = GL_TRIANGLES;

    GLuint texture = 0;
};
