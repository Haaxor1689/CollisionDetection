#include "mesh.hpp"

#include "cube.inl"
#include "sphere.inl"
#include "objects.inl"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

Mesh::Mesh(const std::string& fileName, GLint positionLocation, GLint normalLocation, GLint texCoordLocation) {
    *this = FromFile(fileName, positionLocation, normalLocation, texCoordLocation)[0];
}

Mesh::Mesh(std::vector<float> vertices, std::vector<float> normals, std::vector<float> texCoords,
           std::vector<uint32_t> indices, GLenum mode, GLint positionLocation, GLint normalLocation,
           GLint texCoordLocation) {
    verticesCount = vertices.size() / 3;

    // Create buffer for vertices
    glGenBuffers(1, &verticesBufferId);
    glBindBuffer(GL_ARRAY_BUFFER, verticesBufferId);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Create buffer for normals
    if (!normals.empty()) {
        glGenBuffers(1, &normalsBufferId);
        glBindBuffer(GL_ARRAY_BUFFER, normalsBufferId);
        glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), normals.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    // Create buffer for texture coordinates
    if (!texCoords.empty()) {
        glGenBuffers(1, &texCoordsBufferId);
        glBindBuffer(GL_ARRAY_BUFFER, texCoordsBufferId);
        glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(float), texCoords.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    if (!indices.empty()) {
        // If indices buffer is not empty => create buffer for indices
        glGenBuffers(1, &indicesBufferId);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBufferId);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        indicesCount = indices.size();
    }

    this->mode = mode;

    CreateVao(positionLocation, normalLocation, texCoordLocation);
}

Mesh::Mesh(const Mesh& other) {
    verticesCount = other.verticesCount;
    indicesCount = other.indicesCount;
    mode = other.mode;

    // Copy vertices
    if (other.verticesBufferId != 0) {
        glGenBuffers(1, &verticesBufferId);
        glBindBuffer(GL_ARRAY_BUFFER, verticesBufferId);
        glBufferData(GL_ARRAY_BUFFER, verticesCount * sizeof(float) * 3, nullptr, GL_STATIC_DRAW);

        glBindBuffer(GL_COPY_READ_BUFFER, other.verticesBufferId);
        glBindBuffer(GL_COPY_WRITE_BUFFER, verticesBufferId);
        glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, verticesCount * sizeof(float) * 3);
    }

    // Copy normals
    if (other.normalsBufferId != 0) {
        glGenBuffers(1, &normalsBufferId);
        glBindBuffer(GL_ARRAY_BUFFER, normalsBufferId);
        glBufferData(GL_ARRAY_BUFFER, verticesCount * sizeof(float) * 3, nullptr, GL_STATIC_DRAW);

        glBindBuffer(GL_COPY_READ_BUFFER, other.normalsBufferId);
        glBindBuffer(GL_COPY_WRITE_BUFFER, normalsBufferId);
        glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, verticesCount * sizeof(float) * 3);
    }

    // Copy texture coordinates
    if (other.texCoordsBufferId != 0) {
        glGenBuffers(1, &texCoordsBufferId);
        glBindBuffer(GL_ARRAY_BUFFER, texCoordsBufferId);
        glBufferData(GL_ARRAY_BUFFER, verticesCount * sizeof(float) * 2, nullptr, GL_STATIC_DRAW);

        glBindBuffer(GL_COPY_READ_BUFFER, other.texCoordsBufferId);
        glBindBuffer(GL_COPY_WRITE_BUFFER, texCoordsBufferId);
        glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, verticesCount * sizeof(float) * 2);
    }

    // Copy indices buffer
    if (other.indicesBufferId != 0) {
        glGenBuffers(1, &indicesBufferId);
        glBindBuffer(GL_ARRAY_BUFFER, indicesBufferId);
        glBufferData(GL_ARRAY_BUFFER, indicesCount * sizeof(uint32_t), nullptr, GL_STATIC_DRAW);

        glBindBuffer(GL_COPY_READ_BUFFER, other.indicesBufferId);
        glBindBuffer(GL_COPY_WRITE_BUFFER, indicesBufferId);
        glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, indicesCount * sizeof(uint32_t));
    }

    // Set up VAO if the other object has one too
    if (other.vaoId != 0) {
        CreateVao(other.positionLocation, other.normalLocation, other.texCoordLocation);
    }

    glBindBuffer(GL_COPY_READ_BUFFER, 0);
    glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mesh::CreateVao(GLint positionLocation, GLint normalLocation, GLint texCoordLocation) {
    // In case we want recreate it
    glDeleteVertexArrays(1, &vaoId);

    // Create a vertex array object for the geometry
    glGenVertexArrays(1, &vaoId);

    // Set the parameters of the geometry
    glBindVertexArray(vaoId);

    if (positionLocation >= 0) {
        this->positionLocation = positionLocation;
        glBindBuffer(GL_ARRAY_BUFFER, verticesBufferId);
        glEnableVertexAttribArray(positionLocation);
        glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, nullptr);
    }
    if (normalLocation >= 0) {
        this->normalLocation = normalLocation;
        glBindBuffer(GL_ARRAY_BUFFER, normalsBufferId);
        glEnableVertexAttribArray(normalLocation);
        glVertexAttribPointer(normalLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, nullptr);
    }
    if (texCoordLocation >= 0) {
        this->texCoordLocation = texCoordLocation;
        glBindBuffer(GL_ARRAY_BUFFER, texCoordsBufferId);
        glEnableVertexAttribArray(texCoordLocation);
        glVertexAttribPointer(texCoordLocation, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, nullptr);
    }

    if (indicesBufferId != 0) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBufferId);
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::Draw() const {
    BindVao();
    glBindTexture(GL_TEXTURE_2D, texture);

    if (indicesBufferId > 0) {
        glDrawElements(mode, static_cast<GLsizei>(indicesCount), GL_UNSIGNED_INT, nullptr);
    } else {
        glDrawArrays(mode, 0, static_cast<GLsizei>(verticesCount));
    }
}

Mesh Mesh::FromInterleaved(std::vector<float> interleavedVertices, std::vector<uint32_t> indices, GLenum mode,
                           GLint positionLocation, GLint normalLocation, GLint texCoordLocation) {
    // Deinterleave and then call deinterleaved constructor
    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<float> texCoords;

    for (size_t vertexOffset = 0; vertexOffset < interleavedVertices.size(); vertexOffset += 8) {
        for (int i = 0; i < 3; i++) {
            vertices.push_back(interleavedVertices[vertexOffset + i]);
            normals.push_back(interleavedVertices[vertexOffset + 3 + i]);
        }

        texCoords.push_back(interleavedVertices[vertexOffset + 6]);
        texCoords.push_back(interleavedVertices[vertexOffset + 7]);
    }

    Mesh mesh(vertices, normals, texCoords, indices, mode);
    mesh.CreateVao(positionLocation, normalLocation, texCoordLocation);

    return mesh;
}

Mesh Mesh::Cube(GLint positionLocation, GLint normalLocation, GLint texCoordLocation) {
    return FromInterleaved(std::vector<float>(cube_vertices, std::end(cube_vertices)),
                           std::vector<uint32_t>(cube_indices, std::end(cube_indices)), GL_TRIANGLES,
                           positionLocation, normalLocation, texCoordLocation);
}

Mesh Mesh::Sphere(GLint positionLocation, GLint normalLocation, GLint texCoordLocation) {
    return FromInterleaved(std::vector<float>(sphere_vertices, std::end(sphere_vertices)),
                           std::vector<uint32_t>(sphere_indices, std::end(sphere_indices)), GL_TRIANGLE_STRIP,
                           positionLocation, normalLocation, texCoordLocation);
}

Mesh Mesh::Ground(GLint positionLocation, GLint normalLocation, GLint texCoordLocation) {
    return FromInterleaved(GROUND_VERTICES,
                           {}, GL_TRIANGLE_FAN,
                           positionLocation, normalLocation, texCoordLocation);
}

Mesh Mesh::Pad(GLint positionLocation, GLint normalLocation, GLint texCoordLocation) {
    return FromInterleaved(PAD_VERTICES,
                           {}, GL_TRIANGLE_STRIP,
                           positionLocation, normalLocation, texCoordLocation);
}

Mesh Mesh::Brick(GLint positionLocation, GLint normalLocation, GLint texCoordLocation) {
    return FromInterleaved(BRICK_VERTICES,
                           {}, GL_TRIANGLE_STRIP,
                           positionLocation, normalLocation, texCoordLocation);
}

std::vector<Mesh> Mesh::FromFile(const std::string& fileName, GLint positionLocation, GLint normalLocation, GLint texCoordLocation) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string err;
    if (!LoadObj(&attrib, &shapes, &materials, &err, fileName.c_str())) {
        throw std::runtime_error("Could not load object file.");
    }

    std::vector<Mesh> meshes;

    // Loop over shapes
    for (size_t s = 0; s < shapes.size(); s++) {
        std::vector<float> vertices;
        std::vector<float> normals;
        std::vector<float> texCoords;

        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            // Loop over vertices in the face/polygon
            // (because of triangulate true in LoadObj number of vertices
            // per face/polygon is always 3
            for (size_t v = 0; v < 3; v++) {
                // access to vertex
                auto idx = shapes[s].mesh.indices[f * 3 + v];

                for (auto i = 0; i < 3; i++) {
                    vertices.push_back(attrib.vertices[3 * idx.vertex_index + i]);

                    if (!attrib.normals.empty()) {
                        vertices.push_back(attrib.normals[3 * idx.normal_index + i]);
                    }
                }

                if (!attrib.texcoords.empty()) {

                    texCoords.push_back(attrib.texcoords[2 * idx.texcoord_index + 0]);
                    texCoords.push_back(attrib.texcoords[2 * idx.texcoord_index + 1]);
                }
            }
        }

        meshes.emplace_back(vertices, normals, texCoords, std::vector<uint32_t>());
        meshes[meshes.size() - 1].CreateVao(positionLocation, normalLocation, texCoordLocation);
    }

    return meshes;
}

Mesh::~Mesh() {
    // Deallocate all resources on GPU
    // Everything can be deleted without checking because we initialize it
    // to 0 and specification ignores 0 in glDelete* calls
    glDeleteVertexArrays(1, &vaoId);

    glDeleteBuffers(1, &verticesBufferId);
    glDeleteBuffers(1, &normalsBufferId);
    glDeleteBuffers(1, &texCoordsBufferId);
    glDeleteBuffers(1, &indicesBufferId);
}
