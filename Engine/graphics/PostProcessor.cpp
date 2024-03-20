#include "PostProcessor.h"

#include "FrameBuffer.h"
#include "utils/Shader.h"
#include <memory>

PostProcess::PostProcess(int width, int height, const std::shared_ptr<Shader> &shader,
                         bool multisampled)
        : width(width), height(height), shader(shader) {
    frameBuffer = std::make_shared<FrameBuffer>(width, height, multisampled);
    shader->use();
    shader->setUniform("screenTexture", 0);
}

void PostProcess::render() {
    glDisable(GL_DEPTH_TEST);
    shader->use();
    renderPlane.draw(frameBuffer->getTexture());
    frameBuffer->unbind();
    glEnable(GL_DEPTH_TEST);
}

void PostProcess::begin() { frameBuffer->bind(); }

void PostProcess::end() { frameBuffer->unbind(); }

void PostProcess::setShader(std::shared_ptr<Shader> shader) {
    this->shader = shader;
    shader->use();
    shader->setUniform("screenTexture", 0);
}

auto PostProcess::getShader() -> std::shared_ptr<Shader> { return shader; }

auto PostProcess::getFrameBuffer() -> std::shared_ptr<FrameBuffer> {
    return frameBuffer;
}

void PostProcess::setWidth(int width) {
    this->width = width;
    frameBuffer->setWidth(width);
}

void PostProcess::setHeight(int height) {
    this->height = height;
    frameBuffer->setHeight(height);
}

void PostProcess::resize(int width, int height) {
    this->width = width;
    this->height = height;
    frameBuffer->resize(width, height);
}
