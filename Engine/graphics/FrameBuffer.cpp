//
// Created by Jacob Edwards on 24/03/2024.
//

#include "FrameBuffer.h"

#include "Config.h"
#include <GL/glew.h>
#include <stdexcept>
#include <iostream>

FrameBuffer::FrameBuffer(unsigned int width,
                         unsigned int height, bool multisample)
        : width(width), height(height), multisample(multisample) {
    setup();
}

FrameBuffer::~FrameBuffer() {
    glDeleteFramebuffers(1, &FBO);
    glDeleteRenderbuffers(1, &RBO);

    if (multisample) {
        glDeleteFramebuffers(1, &MSFBO);
        glDeleteRenderbuffers(1, &MSRBO);
        glDeleteRenderbuffers(1, &MSRBO_depth);
    }
}


FrameBuffer::FrameBuffer(FrameBuffer &&other) noexcept {
    FBO = other.FBO;
    RBO = other.RBO;
    texture = other.texture;
    MSFBO = other.MSFBO;
    MSRBO = other.MSRBO;
    MSRBO_depth = other.MSRBO_depth;
    previousFBO = other.previousFBO;
    width = other.width;
    height = other.height;
    multisample = other.multisample;

    other.FBO = 0;
    other.RBO = 0;
    other.texture.id = 0;
    other.MSFBO = 0;
    other.MSRBO = 0;
    other.MSRBO_depth = 0;
    other.previousFBO = 0;
    other.width = 0;
    other.height = 0;
    other.multisample = false;
}

auto FrameBuffer::operator=(FrameBuffer &&other) noexcept -> FrameBuffer & {
    if (this != &other) {
        glDeleteFramebuffers(1, &FBO);
        glDeleteRenderbuffers(1, &RBO);
        glDeleteFramebuffers(1, &MSFBO);
        glDeleteRenderbuffers(1, &MSRBO);
        glDeleteRenderbuffers(1, &MSRBO_depth);

        FBO = other.FBO;
        RBO = other.RBO;
        texture = other.texture;
        MSFBO = other.MSFBO;
        MSRBO = other.MSRBO;
        MSRBO_depth = other.MSRBO_depth;
        previousFBO = other.previousFBO;
        width = other.width;
        height = other.height;
        multisample = other.multisample;

        other.FBO = 0;
        other.RBO = 0;
        other.texture.id = 0;
        other.MSFBO = 0;
        other.MSRBO = 0;
        other.MSRBO_depth = 0;
        other.previousFBO = 0;
        other.width = 0;
        other.height = 0;
        other.multisample = false;
    }
    return *this;
}

void FrameBuffer::bind() {
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, reinterpret_cast<GLint *>(&previousFBO));

    if (multisample) {
        glBindFramebuffer(GL_FRAMEBUFFER, MSFBO);
    } else {
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    }

    glViewport(0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height));
    glClearColor(0.0F, 0.0F, 0.0F, 1.0F);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void FrameBuffer::unbind() const {

    if (multisample) {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, MSFBO);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);
        glBlitFramebuffer(0, 0, static_cast<GLint>(width), static_cast<GLint>(height), 0, 0,
                          static_cast<GLint>(width), static_cast<GLint>(height),
                          GL_COLOR_BUFFER_BIT, GL_NEAREST);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, previousFBO);

    glViewport(0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height));
}

[[nodiscard]] auto FrameBuffer::getTexture() const -> Texture::Data { return texture; }

void FrameBuffer::setHeight(unsigned int height) {
    this->height = height;
    resize();
}

void FrameBuffer::setWidth(unsigned int width) {
    this->width = width;
    resize();
}

void FrameBuffer::resize(unsigned int width, unsigned int height) {
    this->width = width;
    this->height = height;
    resize();
}

void FrameBuffer::setup() {
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, static_cast<GLsizei>(width),
                          static_cast<GLsizei>(height));
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                              GL_RENDERBUFFER, RBO);
    texture.bind();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, GL_RGB,
                 GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           texture.id, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        throw std::runtime_error("Framebuffer is not complete");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if (multisample) {
        glGenFramebuffers(1, &MSFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, MSFBO);
        glGenRenderbuffers(1, &MSRBO);
        glBindRenderbuffer(GL_RENDERBUFFER, MSRBO);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_RGBA8, static_cast<GLsizei>(width),
                                         static_cast<GLsizei>(height));
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                  GL_RENDERBUFFER, MSRBO);
        glGenRenderbuffers(1, &MSRBO_depth);
        glBindRenderbuffer(GL_RENDERBUFFER, MSRBO_depth);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8,
                                         static_cast<GLsizei>(width), static_cast<GLsizei>(height));
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                                  GL_RENDERBUFFER, MSRBO_depth);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "Framebuffer is not complete!" << std::endl;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

void FrameBuffer::resize() const {
    if (multisample) {
        glBindFramebuffer(GL_FRAMEBUFFER, MSFBO);
        glBindRenderbuffer(GL_RENDERBUFFER, MSRBO);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_RGBA8, static_cast<GLsizei>(width),
                                         static_cast<GLsizei>(height));
        glBindRenderbuffer(GL_RENDERBUFFER, MSRBO_depth);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8,
                                         static_cast<GLsizei>(width), static_cast<GLsizei>(height));
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, static_cast<GLsizei>(width),
                          static_cast<GLsizei>(height));

    texture.bind();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, GL_RGB,
                 GL_UNSIGNED_BYTE, nullptr);
}
