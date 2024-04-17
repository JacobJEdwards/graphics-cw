#ifndef POSTPROCESS_H
#define POSTPROCESS_H

#include "graphics/FrameBuffer.h"
#include "shapes/Plane.h"
#include "utils/Shader.h"
#include <memory>

constexpr float BLOOM_THRESHOLD = 0.8F;
constexpr float BLOOM_INTENSITY = 1.4F;
constexpr float VIGNETTE_STRENGTH = 0.3F;
constexpr float GAMMA = 1.0F;
constexpr float EXPOSURE = 1.0F;
constexpr float CONTRAST = 1.2F;
constexpr float SATURATION = 1.5F;
constexpr float BRIGHTNESS = 1.0F;

constexpr float BLOOM_THRESHOLD_MIN = 0.0F;
constexpr float BLOOM_THRESHOLD_MAX = 1.0F;

constexpr float BLOOM_INTENSITY_MIN = 0.0F;
constexpr float BLOOM_INTENSITY_MAX = 2.0F;

constexpr float VIGNETTE_STRENGTH_MIN = 0.0F;
constexpr float VIGNETTE_STRENGTH_MAX = 1.0F;

constexpr float GAMMA_MIN = 1.0F;
constexpr float GAMMA_MAX = 3.0F;

constexpr float EXPOSURE_MIN = 0.0F;
constexpr float EXPOSURE_MAX = 2.0F;

constexpr float CONTRAST_MIN = 0.0F;
constexpr float CONTRAST_MAX = 2.0F;

constexpr float SATURATION_MIN = 0.0F;
constexpr float SATURATION_MAX = 2.0F;

constexpr float BRIGHTNESS_MIN = 0.0F;
constexpr float BRIGHTNESS_MAX = 2.0F;


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

    float gamma = GAMMA;
    float exposure = EXPOSURE;
    float contrast = CONTRAST;
    float saturation = SATURATION;
    float brightness = BRIGHTNESS;
    float bloomThreshold = BLOOM_THRESHOLD;
    float bloomIntensity = BLOOM_INTENSITY;
    float vignetteStrength = VIGNETTE_STRENGTH;

    bool blur = false;
    float blurTime = 0.0F;
    float blurAmount = 0.005F;
};

#endif // POSTPROCESS_H
