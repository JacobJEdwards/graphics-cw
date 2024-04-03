//
// Created by Jacob Edwards on 02/03/2024.
//

#ifndef CW_BUFFER_H
#define CW_BUFFER_H

#include "utils/Vertex.h"
#include <GL/glew.h>
#include <initializer_list>
#include <vector>

#include <span>

class Buffer {
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

    Buffer();

    ~Buffer();

    Buffer(const Buffer &other);

    auto operator=(const Buffer &other) -> Buffer &;

    Buffer(Buffer &&other) noexcept;

    auto operator=(Buffer &&other) noexcept -> Buffer &;

    void fill(std::initializer_list<Vertex::Data> vertices, std::initializer_list<GLuint> indices);

    void fill(std::span<const Vertex::Data> vertices, std::span<const GLuint> indices);

    void fill(std::initializer_list<Vertex::Data> vertices);

    void fill(std::span<const Vertex::Data> vertices);

    void bind() const;

    void unbind() const;

    void draw() const;

    void drawInstanced(std::size_t num) const;

private:
    void setup();
};

#endif // CW_BUFFER_H
