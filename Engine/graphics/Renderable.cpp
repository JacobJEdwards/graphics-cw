//
// Created by Jacob Edwards on 23/03/2024.
//

#include "Renderable.h"
#include "utils/Shader.h"
#include <utility>
#include <memory>

void Renderable::setShader(std::shared_ptr<Shader> shader) {
    Renderable::shader = std::move(shader);
}

[[nodiscard]] auto Renderable::getShader() const -> std::shared_ptr<Shader> {
    return shader;
}