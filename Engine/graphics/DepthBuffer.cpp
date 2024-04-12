//
// Created by Jacob Edwards on 20/03/2024.
//

#include "DepthBuffer.h"

#include <iostream>

DepthBuffer::DepthBuffer(const unsigned int width, const unsigned int height) {
    glGenFramebuffers(1, &DBO);
    glBindFramebuffer(GL_FRAMEBUFFER, DBO);

    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, static_cast<GLsizei>(width),
                          static_cast<GLsizei>(height));
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer is not complete!" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Store the dimensions
    this->width = width;
    this->height = height;
}

DepthBuffer::~DepthBuffer() {
    destroy();
}

void DepthBuffer::bind() {
    GLint temp;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &temp);
    glBindFramebuffer(GL_FRAMEBUFFER, DBO);
    glViewport(0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height));
    glClear(GL_DEPTH_BUFFER_BIT);

    previousFBO = static_cast<GLuint>(temp);
}

void DepthBuffer::unbind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, previousFBO);
}

void DepthBuffer::Clear() {
    glClear(GL_DEPTH_BUFFER_BIT);
}

void DepthBuffer::destroy() const {
    glDeleteFramebuffers(1, &DBO);
    glDeleteRenderbuffers(1, &RBO);
}

[[nodiscard]] auto DepthBuffer::getDBO() const -> GLuint {
    return DBO;
}