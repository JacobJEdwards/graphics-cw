//
// Created by Jacob Edwards on 02/03/2024.
//

#include "graphics/buffers/Buffer.h"
#include <cstddef>
#include <initializer_list>
#include <span>
#include <GL/glew.h>
#include <utility>
#include <vector>
#include "graphics/Vertex.h"

Buffer::Buffer() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
}

Buffer::~Buffer() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}


Buffer::Buffer(const Buffer &other) {
    if (this != &other) {
        data = other.data;
        drawMode = other.drawMode;
        setup();
    }
}

auto Buffer::operator=(const Buffer &other) -> Buffer & {
    if (this != &other) {
        data = other.data;
        drawMode = other.drawMode;
        setup();
    }
    return *this;
}


Buffer::Buffer(Buffer &&other) noexcept {
    data = std::move(other.data);
    drawMode = other.drawMode;
    VAO = other.VAO;
    VBO = other.VBO;
    EBO = other.EBO;

    other.VAO = 0U;
    other.VBO = 0U;
    other.EBO = 0U;
}


auto Buffer::operator=(Buffer &&other) noexcept -> Buffer & {
    if (this != &other) {
        data = std::move(other.data);
        drawMode = other.drawMode;
        VAO = other.VAO;
        VBO = other.VBO;
        EBO = other.EBO;

        other.VAO = 0U;
        other.VBO = 0U;
        other.EBO = 0U;
    }

    return *this;
}


void Buffer::fill(const std::initializer_list<Vertex::Data> vertices,
                  const std::initializer_list<GLuint> indices) {
    data.vertices = std::vector(vertices.begin(), vertices.end());
    data.indices = std::vector(indices.begin(), indices.end());

    setup();
}

void Buffer::fill(std::span<const Vertex::Data> vertices,
                  std::span<const GLuint> indices) {
    data.vertices = std::vector(vertices.begin(), vertices.end());
    data.indices = std::vector(indices.begin(), indices.end());

    setup();
}

void Buffer::fill(const std::initializer_list<Vertex::Data> vertices) {
    data.vertices = std::vector(vertices.begin(), vertices.end());
    setup();
}

void Buffer::fill(std::span<const Vertex::Data> vertices) {
    data.vertices = std::vector(vertices.begin(), vertices.end());
    setup();
}

void Buffer::bind() const {
    glBindVertexArray(VAO);
}

void Buffer::unbind() const {
    glBindVertexArray(0);
}

void Buffer::draw() const {
    if (!data.indices.empty()) {
        glDrawElements(drawMode, static_cast<GLsizei>(data.indices.size()),
                       GL_UNSIGNED_INT, nullptr);
    } else {
        glDrawArrays(drawMode, 0, static_cast<GLsizei>(data.vertices.size()));
    }
}

void Buffer::setup() const {
    bind();
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(data.vertices.size() * sizeof(Vertex::Data)),
        data.vertices.data(), GL_STATIC_DRAW);

    if (!data.indices.empty()) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     static_cast<GLsizeiptr>(data.indices.size() * sizeof(GLuint)),
                     data.indices.data(), GL_STATIC_DRAW);
    }

    // vertex positions
    glEnableVertexAttribArray(Vertex::Layout::POSITION);
    glVertexAttribPointer(Vertex::Layout::POSITION, 3, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex::Data), nullptr);

    // vertex normals
    glEnableVertexAttribArray(Vertex::Layout::NORMAL);
    glVertexAttribPointer(
        Vertex::Layout::NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex::Data),
        reinterpret_cast<void *>(offsetof(Vertex::Data, normal)));

    // vertex texture coords
    glEnableVertexAttribArray(Vertex::Layout::TEX_COORDS);
    glVertexAttribPointer(
        Vertex::Layout::TEX_COORDS, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex::Data),
        reinterpret_cast<void *>(offsetof(Vertex::Data, texCoords)));

    // vertex tangent
    glEnableVertexAttribArray(Vertex::Layout::TANGENT);
    glVertexAttribPointer(
        Vertex::Layout::TANGENT, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex::Data),
        reinterpret_cast<void *>(offsetof(Vertex::Data, tangent)));

    // vertex bitangent
    glEnableVertexAttribArray(Vertex::Layout::BITANGENT);
    glVertexAttribPointer(
        Vertex::Layout::BITANGENT, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex::Data),
        reinterpret_cast<void *>(offsetof(Vertex::Data, bitangent)));
    unbind();
}

void Buffer::drawInstanced(const std::size_t num) const {
    if (!data.indices.empty()) {
        glDrawElementsInstanced(drawMode, static_cast<GLsizei>(data.indices.size()), GL_UNSIGNED_INT, nullptr,
                                static_cast<GLsizei>(num));
    } else {
        glDrawArraysInstanced(drawMode, 0, static_cast<GLsizei>(data.vertices.size()), static_cast<GLsizei>(num));
    }
}