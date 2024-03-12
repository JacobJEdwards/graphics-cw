//
// Created by Jacob Edwards on 02/03/2024.
//

#ifndef CW_BUFFER_H
#define CW_BUFFER_H

#include <initializer_list>
#include <vector>
#include <GL/glew.h>
#include <span>
#include <utility>

#include "Vertex.h"

struct BufferData {
    std::vector<Vertex::Data> vertices;
    std::vector<GLuint> indices;

    BufferData() = default;

    BufferData(std::initializer_list<Vertex::Data> vertices, std::initializer_list<GLuint> indices) : vertices(vertices), indices(indices) {}

    BufferData(std::vector<Vertex::Data> vertices, std::vector<GLuint> indices) : vertices(std::move(vertices)), indices(std::move(indices)) {}
};

struct Buffer {
    GLuint VAO = 0;
    GLuint VBO = 0;
    GLuint EBO = 0;

    Buffer() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
    }

    ~Buffer() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }

    void bind() const {
        glBindVertexArray(VAO);
    }

    void unbind() const {
        glBindVertexArray(0);
    }

    void fill(const BufferData &data) const;
};

#endif //CW_BUFFER_H
