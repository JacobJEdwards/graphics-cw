//
// Created by Jacob Edwards on 20/03/2024.
//

#ifndef CW_SHADOWBUFFER_H
#define CW_SHADOWBUFFER_H

#include <GL/glew.h>

class ShadowBuffer {
public:
    ShadowBuffer(unsigned int width, unsigned int height);

    ~ShadowBuffer();

    ShadowBuffer(const ShadowBuffer &) = delete;

    auto operator=(const ShadowBuffer &) -> ShadowBuffer & = delete;

    void bind();

    void unbind() const;

    static void Clear();

    void destroy();

    [[nodiscard]] auto getTexture() const -> GLuint;

private:
    GLuint depthTexture = 0;
    GLuint FBO = 0;
    GLuint previousFBO = 0;
    unsigned int width;
    unsigned int height;
};


#endif //CW_SHADOWBUFFER_H
