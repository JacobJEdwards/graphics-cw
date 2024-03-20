//
// Created by Jacob Edwards on 20/03/2024.
//

#ifndef CW_DEPTHBUFFER_H
#define CW_DEPTHBUFFER_H

#include <GL/glew.h>

class DepthBuffer {
public:
    DepthBuffer(int width, int height);

    ~DepthBuffer();

    void bind();

    void unbind() const;

    static void Clear();

    void destroy();

    [[nodiscard]] auto getDBO() const -> GLuint;

private:
    GLuint DBO;
    GLuint RBO;
    GLint previousFBO;
    GLuint depthBuffer;
    int width;
    int height;
};


#endif //CW_DEPTHBUFFER_H
