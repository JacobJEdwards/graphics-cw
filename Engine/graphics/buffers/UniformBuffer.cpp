//
// Created by Jacob Edwards on 08/04/2024.
//

#include <GL/glew.h>
#include "graphics/buffers/UniformBuffer.h"

//
// Created by Jacob Edwards on 08/04/2024.
//

UniformBuffer::UniformBuffer() {
    glGenBuffers(1, &UBO);
    glBindBuffer(GL_UNIFORM_BUFFER, UBO);
    glBufferData(GL_UNIFORM_BUFFER, 0, nullptr, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

UniformBuffer::~UniformBuffer() {
    glDeleteBuffers(1, &UBO);
}

void UniformBuffer::bind() const {
    glBindBuffer(GL_UNIFORM_BUFFER, UBO);
}

void UniformBuffer::unbind() {
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}


