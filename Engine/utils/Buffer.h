//
// Created by Jacob Edwards on 02/03/2024.
//

#ifndef CW_BUFFER_H
#define CW_BUFFER_H

#include <initializer_list>
#include <vector>
#include <GL/glew.h>
#include "utils/Vertex.h"
#include <iostream>

class Buffer {
public:
    GLuint VAO;
    GLuint VBO;
    GLuint EBO;

    struct Data {
        std::vector<Vertex::Data> vertices;
        std::vector<GLuint> indices;
    };

    Data data;

    Buffer() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
    }

    void fill(std::initializer_list<Vertex::Data> vertices, std::initializer_list<GLuint> indices) {
        data.vertices = vertices;
        data.indices = indices;

        setup();
    }

    void fill(std::vector<Vertex::Data> vertices, std::vector<GLuint> indices) {
        data.vertices = vertices;
        data.indices = indices;

        setup();
    }

    void bind() const {
        glBindVertexArray(VAO);
    }

    void unbind() const {
        glBindVertexArray(0);
    }

    void draw() const {
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(data.indices.size()), GL_UNSIGNED_INT, nullptr);
    }

private:
    void setup() {
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
};

#endif //CW_BUFFER_H
