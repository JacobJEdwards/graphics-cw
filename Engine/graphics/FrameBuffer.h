#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "Config.h"
#include <GL/glew.h>
#include "graphics/Texture.h"

class FrameBuffer {
public:
    explicit FrameBuffer(unsigned int width = Config::DEFAULT_WIDTH,
                         unsigned int height = Config::DEFAULT_HEIGHT, bool multisample = false);

    ~FrameBuffer();

    FrameBuffer(const FrameBuffer &other) = delete;

    auto operator=(const FrameBuffer &other) -> FrameBuffer & = delete;

    FrameBuffer(FrameBuffer &&other) noexcept;

    auto operator=(FrameBuffer &&other) noexcept -> FrameBuffer &;

    void bind();

    void unbind() const;

    [[nodiscard]] auto getTexture() const -> GLuint;

    void setHeight(unsigned int height);

    void setWidth(unsigned int width);

    void resize(unsigned int width, unsigned int height);

    void setMultisampled(bool multisampled);

protected:
    void setup();

    void resize() const;

private:
    GLuint FBO = 0;
    GLuint RBO = 0;
    Texture::Data texture;

    // multi sample
    GLuint MSFBO = 0;
    GLuint MSRBO = 0;
    GLuint MSRBO_depth = 0;

    GLuint previousFBO = 0;

    unsigned int width;
    unsigned int height;

    bool multisample;
};

#endif
