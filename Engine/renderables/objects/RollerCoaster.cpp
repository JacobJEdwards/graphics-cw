//
// Created by Jacob Edwards on 28/04/2024.
//

#include "RollerCoaster.h"

#include "graphics/Model.h"
#include "graphics/Shader.h"
#include "utils/ShaderManager.h"
#include "renderables/Renderable.h"
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


RollerCoaster::RollerCoaster() : model("../Assets/objects/rollercoaster/coaster.obj") {
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::scale(modelMatrix, scale);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.x), glm::vec3(1.0F, 0.0F, 0.0F));

    shader = ShaderManager::GetInstance().get("Base");
}

void RollerCoaster::draw(const std::shared_ptr<Shader> shader) const {
    shader->use();
    shader->setUniform("model", modelMatrix);
    model.draw(shader);
}
