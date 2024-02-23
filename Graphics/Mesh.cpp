//
// Created by Jacob Edwards on 21/02/2024.
//

#include "Mesh.h"
#include <vector>
#include <string>
#include <GL/glew.h>
#include <iostream>

#include "Vertex.h"
#include "Texture.h"
#include "Shader.h"

Mesh::Mesh(std::vector<Vertex::Data> vertices, std::vector<GLuint> indices,
           std::vector<Texture::Data> textures) : vertices(std::move(vertices)), indices(std::move(indices)),
                                                  textures(std::move(textures)) {
    setupMesh();
}

void Mesh::draw(const Shader &shader) const {
    GLuint diffuseNr = 1;
    GLuint specularNr = 1;
    GLuint normalNr = 1;
    GLuint heightNr = 1;
    GLuint ambientOcclusionNr = 1;
    GLuint emissiveNr = 1;

    for (GLuint i = 0; i < textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);

        std::string number;
        const Texture::Type name = textures[i].type;

        switch (name) {
            case Texture::Type::DIFFUSE:
                number = std::to_string(diffuseNr);
                diffuseNr++;
                break;
            case Texture::Type::SPECULAR:
                number = std::to_string(specularNr);
                specularNr++;
                break;
            case Texture::Type::NORMAL:
                number = std::to_string(normalNr);
                normalNr++;
                break;
            case Texture::Type::HEIGHT:
                number = std::to_string(heightNr);
                heightNr++;
                break;
            case Texture::Type::AMBIENT_OCCLUSION:
                number = std::to_string(ambientOcclusionNr);
                ambientOcclusionNr++;
                break;
            case Texture::Type::EMISSIVE:
                number = std::to_string(emissiveNr);
                emissiveNr++;
                break;
            default:
                throw std::runtime_error("Unknown texture type");
        }

        glBindTexture(GL_TEXTURE_2D, textures[i].id);
        shader.setInt(("texture_" + toString(name) + number), static_cast<GLint>(i));
    }
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE0);
}

void Mesh::setupMesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(Vertex::Data)), vertices.data(),
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(indices.size() * sizeof(GLuint)), indices.data(),
                 GL_STATIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(Vertex::Layout::POSITION);
    glVertexAttribPointer(Vertex::Layout::POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex::Data), nullptr);

    // vertex normals
    glEnableVertexAttribArray(Vertex::Layout::NORMAL);
    glVertexAttribPointer(Vertex::Layout::NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex::Data),
                          reinterpret_cast<void *>(offsetof(Vertex::Data, normal)));

    // vertex texture coords
    glEnableVertexAttribArray(Vertex::Layout::TEX_COORDS);
    glVertexAttribPointer(Vertex::Layout::TEX_COORDS, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex::Data),
                          reinterpret_cast<void *>(offsetof(Vertex::Data, texCoords)));

    // vertex tangent
    glEnableVertexAttribArray(Vertex::Layout::TANGENT);
    glVertexAttribPointer(Vertex::Layout::TANGENT, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex::Data),
                          reinterpret_cast<void *>(offsetof(Vertex::Data, tangent)));

    // vertex bitangent
    glEnableVertexAttribArray(Vertex::Layout::BITANGENT);
    glVertexAttribPointer(Vertex::Layout::BITANGENT, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex::Data),
                          reinterpret_cast<void *>(offsetof(Vertex::Data, bitangent)));

    glBindVertexArray(0);
}


