//
// Created by Jacob Edwards on 20/03/2024.
//

#include "DepthBuffer.h"

#include <cstdio>
#include <print>

DepthBuffer::DepthBuffer(const unsigned int width, const unsigned int height) {
    glGenFramebuffers(1, &DBO);
    glBindFramebuffer(GL_FRAMEBUFFER, DBO);

    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32F, static_cast<GLsizei>(width),
                          static_cast<GLsizei>(height));
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);

    glGenTextures(1, &texture.id);
    glBindTexture(GL_TEXTURE_2D, texture.id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, static_cast<GLsizei>(width),
                 static_cast<GLsizei>(height), 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture.id, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::println(stderr, "Framebuffer is not complete!");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    this->width = width;
    this->height = height;
}

DepthBuffer::~DepthBuffer() {
    destroy();
}

void DepthBuffer::bind() {
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, reinterpret_cast<GLint *>(&previousFBO));
    glGetIntegerv(GL_VIEWPORT, reinterpret_cast<GLint *>(previousViewport));
    glGetIntegerv(GL_DEPTH_FUNC, reinterpret_cast<GLint *>(&previousDepthFunc));

    glBindFramebuffer(GL_FRAMEBUFFER, DBO);
    glViewport(0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height));
    glClear(GL_DEPTH_BUFFER_BIT);
    glDepthFunc(GL_LESS);
}

void DepthBuffer::unbind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, previousFBO);
    glViewport(previousViewport[0], previousViewport[1], previousViewport[2], previousViewport[3]);
    glDepthFunc(previousDepthFunc);
}

auto DepthBuffer::getTexture() const -> GLuint {
    return texture.id;
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
