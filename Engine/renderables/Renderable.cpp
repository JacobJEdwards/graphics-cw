//
// Created by Jacob Edwards on 23/03/2024.
//

#include "renderables/Renderable.h"
#include "graphics/Shader.h"
#include <glm/ext/matrix_float4x4.hpp>
#include <utility>
#include <memory>

void Renderable::setShader(std::shared_ptr<Shader> shader) {
    Renderable::shader = std::move(shader);
}

[[nodiscard]] auto Renderable::getShader() const -> std::shared_ptr<Shader> {
    return shader;
}

void Renderable::draw() const {
    draw(shader);
}

void Renderable::draw(const glm::mat4 &view, const glm::mat4 &projection) const {
    shader->use();
    shader->setUniform("view", view);
    shader->setUniform("projection", projection);
    draw();
}

void Renderable::draw(const std::shared_ptr<Shader> shader) const {
    shader->use();
    draw();
}
