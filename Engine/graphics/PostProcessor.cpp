#include "PostProcessor.h"

#include "FrameBuffer.h"
#include "utils/Shader.h"
#include <memory>
#include "imgui/imgui.h"
#include "utils/ShaderManager.h"

PostProcess::PostProcess(unsigned int width, unsigned int height,
                         bool multisampled)
        : width(width), height(height) {

    frameBuffer = std::make_shared<FrameBuffer>(width, height, multisampled);

    shader = std::make_shared<Shader>("../Assets/shaders/postProcessing.vert", "../assets/shaders/postProcessing.frag");

    shader->use();
    shader->setUniform("screenTexture", 0);
}

void PostProcess::render() {
    glDisable(GL_DEPTH_TEST);
    shader->use();
    shader->setUniform("gamma", gamma);
    shader->setUniform("exposure", exposure);
    shader->setUniform("contrast", contrast);
    shader->setUniform("saturation", saturation);
    shader->setUniform("brightness", brightness);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, frameBuffer->getTexture());
    renderPlane.draw();
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

void PostProcess::setWidth(unsigned int width) {
    this->width = width;
    frameBuffer->setWidth(width);
}

void PostProcess::setHeight(unsigned int height) {
    this->height = height;
    frameBuffer->setHeight(height);
}

void PostProcess::resize(unsigned int width, unsigned int height) {
    this->width = width;
    this->height = height;
    frameBuffer->resize(width, height);
}

void PostProcess::setMultisampled(bool multisampled) {
    this->multisampled = multisampled;
    frameBuffer->setMultisampled(multisampled);
}

void PostProcess::interface() {
    ImGui::Begin("Post Process");

    if (ImGui::Checkbox("Multisampled", &multisampled)) {
        frameBuffer->setMultisampled(multisampled);
    }

    ImGui::SliderFloat("Gamma", &gamma, 0.0f, 5.0f);
    ImGui::SliderFloat("Exposure", &exposure, 0.0f, 5.0f);
    ImGui::SliderFloat("Contrast", &contrast, 0.0f, 5.0f);
    ImGui::SliderFloat("Saturation", &saturation, 0.0f, 5.0f);
    ImGui::SliderFloat("Brightness", &brightness, 0.0f, 5.0f);
    ImGui::End();
}
