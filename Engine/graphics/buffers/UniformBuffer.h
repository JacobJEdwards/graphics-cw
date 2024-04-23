//
// Created by Jacob Edwards on 08/04/2024.
//

#ifndef CW_UNIFORMBUFFER_H
#define CW_UNIFORMBUFFER_H

#include <cstddef>
#include <vector>
#include <GL/glew.h>

class UniformBuffer {
public:
    explicit UniformBuffer(const size_t size) : size(size) {
        glGenBuffers(1, &id);
        glBindBuffer(GL_UNIFORM_BUFFER, id);
        glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_STATIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    void bind() const {
        glBindBuffer(GL_UNIFORM_BUFFER, id);
    }

    void unbind() const {
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    void setOffset(const GLint offset, const GLint size) {
        offsets.push_back(offset);
        sizes.push_back(size);
    }

    void set(const GLint offset, const void *data, const size_t size) {
        bind();
        glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
        unbind();
    }

    void set(const GLint index, const void *data) {
        set(offsets[index], data, sizes[index]);
    }

    // update the entire buffer
    void update(const void *data) const {
        bind();
        glBufferSubData(GL_UNIFORM_BUFFER, 0, size, data);
        unbind();
    }

    void bindToShader(const GLuint bindingPoint, const GLuint shaderProgram, const std::string &blockName) const {
        const auto blockIndex = glGetUniformBlockIndex(shaderProgram, blockName.c_str());
        glUniformBlockBinding(shaderProgram, blockIndex, bindingPoint);
        glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, id);
    }

private:
    GLuint id;
    size_t size;
    std::vector<GLint> offsets;
    std::vector<GLint> sizes;
};


#endif
