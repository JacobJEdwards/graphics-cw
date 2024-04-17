//
// Created by Jacob Edwards on 20/03/2024.
//

#include "graphics/buffers/ShadowBuffer.h"

#include <iostream>
#include <array>

ShadowBuffer::ShadowBuffer(const unsigned int width, const unsigned int height) {
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0,
                 GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    constexpr std::array<GLfloat, 4> borderColor{1.0, 1.0, 1.0, 1.0};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor.data());

    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer is not complete!" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    this->width = width;
    this->height = height;
}

ShadowBuffer::~ShadowBuffer() {
    destroy();
}

void ShadowBuffer::bind() {
    glGetIntegerv(GL_CULL_FACE_MODE, reinterpret_cast<GLint *>(&previousCullFace));
    glGetIntegerv(GL_DEPTH_FUNC, reinterpret_cast<GLint *>(&previousDepthFunc));
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, reinterpret_cast<GLint *>(&previousFBO));
    glGetIntegerv(GL_VIEWPORT, previousViewport.data());

    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glViewport(0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height));
    glClear(GL_DEPTH_BUFFER_BIT);

    glCullFace(GL_FRONT);
    glDepthFunc(GL_LESS);
}

void ShadowBuffer::unbind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, previousFBO);
    glCullFace(previousCullFace);
    glDepthFunc(previousDepthFunc);
    glViewport(previousViewport[0], previousViewport[1], previousViewport[2], previousViewport[3]);
}

void ShadowBuffer::Clear() {
    glClear(GL_DEPTH_BUFFER_BIT);
}

void ShadowBuffer::destroy() const {
    glDeleteFramebuffers(1, &FBO);
    glDeleteTextures(1, &depthTexture);
}

[[nodiscard]] auto ShadowBuffer::getTexture() const -> GLuint {
    return depthTexture;
}
