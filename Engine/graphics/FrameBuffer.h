#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "Config.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stdexcept>

class FrameBuffer {
public:
  FrameBuffer(int width = Config::DEFAULT_WIDTH,
              int height = Config::DEFAULT_HEIGHT, bool multisample = false)
      : width(width), height(height), multisample(multisample) {
    setup();
  }

  ~FrameBuffer() {
    glDeleteFramebuffers(1, &FBO);
    glDeleteRenderbuffers(1, &RBO);
    glDeleteTextures(1, &texture);

    if (multisample) {
      glDeleteFramebuffers(1, &MSFBO);
      glDeleteRenderbuffers(1, &MSRBO);
      glDeleteRenderbuffers(1, &MSRBO_depth);
    }
  }

  FrameBuffer(const FrameBuffer &other) = delete;
  FrameBuffer &operator=(const FrameBuffer &other) = delete;

  void bind() {
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previousFBO);
    if (multisample) {
      glBindFramebuffer(GL_FRAMEBUFFER, MSFBO);
    } else {
      glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    }
    glViewport(0, 0, width, height);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }

  void unbind() const {

    if (multisample) {
      glBindFramebuffer(GL_READ_FRAMEBUFFER, MSFBO);
      glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);
      glBlitFramebuffer(0, 0, width, height, 0, 0, width, height,
                        GL_COLOR_BUFFER_BIT, GL_NEAREST);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, previousFBO);

    glViewport(0, 0, width, height);
  }

  GLuint getTexture() const { return texture; }

  void setHeight(int height) {
    this->height = height;
    resize();
  }

  void setWidth(int width) {
    this->width = width;
    resize();
  }

  void resize(int width, int height) {
    this->width = width;
    this->height = height;
    resize();
  }

protected:
  void setup() {
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                              GL_RENDERBUFFER, RBO);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           texture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
      throw std::runtime_error("Framebuffer is not complete");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if (multisample) {
      glGenFramebuffers(1, &MSFBO);
      glBindFramebuffer(GL_FRAMEBUFFER, MSFBO);
      glGenRenderbuffers(1, &MSRBO);
      glBindRenderbuffer(GL_RENDERBUFFER, MSRBO);
      glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_RGBA8, width,
                                       height);
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                GL_RENDERBUFFER, MSRBO);
      glGenRenderbuffers(1, &MSRBO_depth);
      glBindRenderbuffer(GL_RENDERBUFFER, MSRBO_depth);
      glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8,
                                       width, height);
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                                GL_RENDERBUFFER, MSRBO_depth);
      if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        throw std::runtime_error("Multisample Framebuffer is not complete");
      }
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
  }

  void resize() {
    if (multisample) {
      glBindFramebuffer(GL_FRAMEBUFFER, MSFBO);
      glBindRenderbuffer(GL_RENDERBUFFER, MSRBO);
      glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_RGBA8, width,
                                       height);
      glBindRenderbuffer(GL_RENDERBUFFER, MSRBO_depth);
      glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8,
                                       width, height);
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, NULL);
  }

  GLuint FBO;
  GLuint RBO;
  GLuint texture;

  // multi sample
  GLuint MSFBO;
  GLuint MSRBO;
  GLuint MSRBO_depth;

  GLint previousFBO;

  int width;
  int height;

  bool multisample;
};

#endif