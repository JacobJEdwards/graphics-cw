//
// Created by Jacob Edwards on 20/03/2024.
//

#ifndef CW_DEPTHBUFFER_H
#define CW_DEPTHBUFFER_H

#include <GL/glew.h>
#include <graphics/Texture.h>

class DepthBuffer {
public:
    DepthBuffer(unsigned int width, unsigned int height);

    ~DepthBuffer();

    void bind();

    void unbind() const;

    static void Clear();

    void destroy() const;

    [[nodiscard]] auto getTexture() const -> GLuint;

    [[nodiscard]] auto getDBO() const -> GLuint;

private:
    GLuint DBO = 0;
    GLuint RBO = 0;
    GLuint previousFBO = 0;
    GLint previousViewport[4] = {0, 0, 0, 0};
    GLuint previousDepthFunc = 0;

    GLuint depthBuffer = 0;
    Texture::Data texture;
    unsigned int width;
    unsigned int height;
};


#endif //CW_DEPTHBUFFER_H
