//
// Created by Jacob Edwards on 02/03/2024.
//

#ifndef CW_BUFFER_H
#define CW_BUFFER_H

#include <initializer_list>
#include <vector>
#include <GL/glew.h>

struct Buffer {
    GLuint index = 0;
    size_t SIZE = 0;

    Buffer() {
        glGenBuffers(1, &index);
    }

    ~Buffer() {
        glDeleteBuffers(1, &index);
    }

    Buffer(const Buffer& other) : SIZE(other.SIZE) {
        glGenBuffers(1, &index);
        glBindBuffer(GL_COPY_READ_BUFFER, other.index);
        glBindBuffer(GL_COPY_WRITE_BUFFER, index);
        glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, SIZE * sizeof(GLfloat));
        glBindBuffer(GL_COPY_READ_BUFFER, 0);
        glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
    }

    auto operator=(const Buffer& other) -> Buffer& {
        if (this != &other) {
            // Release current resources
            glDeleteBuffers(1, &index);
            // Copy data
            glGenBuffers(1, &index);
            glBindBuffer(GL_COPY_READ_BUFFER, other.index);
            glBindBuffer(GL_COPY_WRITE_BUFFER, index);
            glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, SIZE * sizeof(GLfloat));
            glBindBuffer(GL_COPY_READ_BUFFER, 0);
            glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
            SIZE = other.SIZE;
        }
        return *this;
    }

    Buffer(Buffer&& other) noexcept : index(other.index), SIZE(other.SIZE) {
        other.index = 0;
        other.SIZE = 0;
    }

    // Move assignment operator
    auto operator=(Buffer&& other) noexcept -> Buffer& {
        if (this != &other) {
            // Release current resources
            glDeleteBuffers(1, &index);
            // Move data
            index = other.index;
            SIZE = other.SIZE;
            other.index = 0;
            other.SIZE = 0;
        }
        return *this;
    }

    template<typename T>
    explicit Buffer(std::vector<T> buf) : Buffer() {
        fill(buf);
    }

    template<typename T>
    Buffer(std::initializer_list<T> buf) : Buffer() {
        fill<T>(buf);
    }

    template<typename T>
    explicit Buffer(size_t size, T* data) : Buffer() {
        fill(size, data);
    }

    template<typename T> void fill(size_t size, T* data);
    template<typename T> void fill(std::vector<T> buf);
    template<typename T> void fill(T val);
    template<typename T> void retrieve(size_t size, T* data);
    template<typename T> void retrieve(std::vector<T>& buf);
    template<typename T> void retrieve(T& val);

};

template<typename T>
void Buffer::fill(size_t size, T* data) {
    glBindBuffer(GL_ARRAY_BUFFER, index);
    glBufferData(GL_ARRAY_BUFFER, size * sizeof(T), data, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    SIZE = size;
}


template<typename T>
void Buffer::fill(T val) {
    fill(1, &val);
}

template<typename T>
void Buffer::fill(std::vector<T> buf) {
    fill(buf.size(), buf.data());
}

template<typename T>
void Buffer::retrieve(size_t size,  T* data) {
    glBindBuffer(GL_ARRAY_BUFFER, index);
    glGetBufferSubData(GL_ARRAY_BUFFER, 0, size*sizeof(T), data);
}

template<typename T>
void Buffer::retrieve(std::vector<T> &buf) {
    retrieve(buf.size(), buf.data());
}

template<typename T>
void Buffer::retrieve(T &val) {
    retrieve(1, &val);
}

#endif //CW_BUFFER_H
