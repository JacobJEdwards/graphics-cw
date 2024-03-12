//
// Created by Jacob Edwards on 02/03/2024.
//

#include "Buffer.h"



Buffer::Buffer() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
}


void Buffer::fill(std::span<const Vertex::Data> vertices, std::span<const GLuint> indices) {
    data.vertices = std::vector<Vertex::Data>(vertices.begin(), vertices.end());
    data.indices = std::vector<GLuint>(indices.begin(), indices.end());

    setup3D();
}

void Buffer::fill(std::span<const Vertex::Data2D> vertices, std::span<const GLuint> indices) {
    data2D.vertices = std::vector<Vertex::Data2D>(vertices.begin(), vertices.end());
    data2D.indices = std::vector<GLuint>(indices.begin(), indices.end());

    setup2D();
}

void Buffer::bind() const {
    glBindVertexArray(VAO);
}

void Buffer::unbind() const {
    glBindVertexArray(0);
}

void Buffer::draw() const {
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(data.indices.size()), GL_UNSIGNED_INT, nullptr);
}

void Buffer::setup3D() {
    bind();
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(data.vertices.size() * sizeof(Vertex::Data)),
                 data.vertices.data(),
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(data.indices.size() * sizeof(GLuint)),
                 data.indices.data(),
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
    unbind();
}

void Buffer::setup2D() {
    bind();
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(data2D.vertices.size() * sizeof(Vertex::Data2D)),
                 data2D.vertices.data(),
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(data2D.indices.size() * sizeof(GLuint)),
                 data2D.indices.data(),
                 GL_STATIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(Vertex::Layout2D::POSITION);
    glVertexAttribPointer(Vertex::Layout2D::POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex::Data2D), nullptr);

    // vertex texture coords
    glEnableVertexAttribArray(Vertex::Layout2D::TEX_COORDS);
    glVertexAttribPointer(Vertex::Layout2D::TEX_COORDS, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex::Data2D),
                          reinterpret_cast<void *>(offsetof(Vertex::Data2D, texCoords)));
    unbind();
}
