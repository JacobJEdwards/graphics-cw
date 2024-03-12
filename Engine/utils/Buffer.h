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

#include <span>

class Buffer {
public:
    GLuint VAO;
    GLuint VBO;
    GLuint EBO;

    struct Data {
        std::vector<Vertex::Data> vertices;
        std::vector<GLuint> indices;
    };

    struct Data2D {
        std::vector<Vertex::Data2D> vertices;
        std::vector<GLuint> indices;
    };

    Data data;
    Data2D data2D;

    Buffer();

    void fill(std::span<const Vertex::Data> vertices, std::span<const GLuint> indices);
    void fill(std::span<const Vertex::Data2D> vertices, std::span<const GLuint> indices);

    void bind() const;

    void unbind() const;

    void draw() const;

private:
    void setup3D();
    void setup2D();
};

#endif //CW_BUFFER_H
