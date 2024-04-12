#ifndef POSTPROCESS_H
#define POSTPROCESS_H

#include "graphics/FrameBuffer.h"
#include "shapes/Plane.h"
#include "utils/Shader.h"
#include <memory>

class PostProcess {
public:
    PostProcess(unsigned int width, unsigned int height,
                bool multisampled = true);

    void render(float deltaTime, float time);

    void begin() const;

    void end() const;

    void setShader(const std::shared_ptr<Shader> &shader);

    auto getShader() -> std::shared_ptr<Shader>;

    auto getFrameBuffer() -> std::shared_ptr<FrameBuffer>;

    void setWidth(unsigned int width);

    void setHeight(unsigned int height);

    void resize(unsigned int width, unsigned int height);

    void interface();

    void setMultisampled(bool multisampled);

    void isBlurred() {
        this->blur = true;
        this->blurTime = 0.8F;
    }

private:
    std::shared_ptr<Shader> shader;
    std::shared_ptr<FrameBuffer> frameBuffer;

    Plane renderPlane;

    unsigned int width;
    unsigned int height;

    bool multisampled = true;

    float gamma = 2.2F;
    float exposure = 1.0F;
    float contrast = 1.2F;
    float saturation = 1.5F;
    float brightness = 1.0F;
    float bloomThreshold = 0.8F;
    float bloomIntensity = 1.4F;
    float vignetteStrength = 0.3F;

    bool blur = false;
    float blurTime = 0.0F;
    float blurAmount = 0.005F;
};

#endif // POSTPROCESS_H
