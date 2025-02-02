//
// Created by Jacob Edwards on 02/03/2024.
//

#ifndef CW_BUFFER_H
#define CW_BUFFER_H

#include "graphics/Vertex.h"
#include <GL/glew.h>
#include <cstddef>
#include <initializer_list>
#include <vector>

#include <span>

class VertexBuffer {
public:
    GLuint VAO = 0;
    GLuint VBO = 0;
    GLuint EBO = 0;

    GLenum drawMode = GL_TRIANGLES;

    struct Data {
        std::vector<Vertex::Data> vertices;
        std::vector<GLuint> indices;
    };

    Data data;

    VertexBuffer();

    ~VertexBuffer();

    VertexBuffer(const VertexBuffer &other);

    auto operator=(const VertexBuffer &other) -> VertexBuffer &;

    VertexBuffer(VertexBuffer &&other) noexcept;

    auto operator=(VertexBuffer &&other) noexcept -> VertexBuffer &;

    void fill(std::initializer_list<Vertex::Data> vertices, std::initializer_list<GLuint> indices);

    void fill(std::span<const Vertex::Data> vertices, std::span<const GLuint> indices);

    void fill(std::initializer_list<Vertex::Data> vertices);

    void fill(std::span<const Vertex::Data> vertices);

    void bind() const;

    void unbind() const;

    void draw() const;

    template<typename T>
    void setInstanceData(const std::size_t index, const T &data) const {
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, index * sizeof(T), sizeof(T), &data);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void drawInstanced(std::size_t num) const;

private:
    void setup() const;
};

#endif // CW_BUFFER_H
