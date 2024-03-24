#ifndef POSTPROCESS_H
#define POSTPROCESS_H

#include "graphics/FrameBuffer.h"
#include "shapes/Plane.h"
#include "utils/Shader.h"
#include <memory>

class PostProcess {
public:
    PostProcess(unsigned int width, unsigned int height, const std::shared_ptr<Shader> &shader,
                bool multisampled = true);

    void render();

    void begin();

    void end();

    void setShader(std::shared_ptr<Shader> shader);

    auto getShader() -> std::shared_ptr<Shader>;

    auto getFrameBuffer() -> std::shared_ptr<FrameBuffer>;

    void setWidth(unsigned int width);

    void setHeight(unsigned int height);

    void resize(unsigned int width, unsigned int height);

private:
    std::shared_ptr<Shader> shader;
    std::shared_ptr<FrameBuffer> frameBuffer;

    Plane renderPlane;

    unsigned int width;
    unsigned int height;
};

#endif // POSTPROCESS_H
