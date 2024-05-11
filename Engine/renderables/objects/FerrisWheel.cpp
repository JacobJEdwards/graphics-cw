//
// Created by Jacob Edwards on 28/04/2024.
//

#include "FerrisWheel.h"

#include "graphics/Shader.h"
#include <utils/ShaderManager.h>

#include "renderables/Renderable.h"
#include "graphics/Model.h"
#include <memory>
#include <glm/ext/matrix_transform.hpp>
#include <graphics/Color.h>

FerrisWheel::FerrisWheel() : Entity("../Assets/objects/ferris/ferris-static.obj"),
                             staticPart("../Assets/objects/ferris/ferris-static.obj"),
                             rotatingPart("../Assets/objects/ferris/ferris-moving.obj"),
                             cabin("../Assets/objects/ferris/ferris-cart.obj"),
                             entrance("../Assets/objects/enterance/enterance.obj") {
    shader = ShaderManager::GetInstance().get("Untextured");

    staticPartTransform = glm::scale(staticPartTransform, scale);
    rotatingPartTransform = glm::scale(rotatingPartTransform, scale);
    cabinTransform = glm::scale(cabinTransform, scale);

    staticPartTransform = glm::translate(staticPartTransform, translation);
    rotatingPartTransform = glm::translate(rotatingPartTransform, translation);
    cabinTransform = glm::translate(cabinTransform, translation);

    entranceTransform = glm::translate(entranceTransform, glm::vec3(-10.0F, 0.0F, 40.0F));

    attributes.mass = 1000.0F;
    attributes.gravityAffected = false;
}

void FerrisWheel::draw(const std::shared_ptr<Shader> shader) const {
    shader->use();
    shader->setUniform("color", Color::WHITE);
    shader->setUniform("model", staticPartTransform);
    staticPart.draw(shader);

    shader->setUniform("model", rotatingPartTransform);
    rotatingPart.draw(shader);

    // shader->setUniform("model", cabinTransform);
    // cabin.draw(shader);
}

void FerrisWheel::update(const float deltaTime) {
    const glm::mat4 translationToCenter = glm::translate(glm::mat4(1.0F), -centre);
    const glm::mat4 rotation = glm::rotate(glm::mat4(1.0F), deltaTime, glm::vec3(1.0F, 0.0F, 0.0F));
    const glm::mat4 translationBack = glm::translate(glm::mat4(1.0F), centre);

    rotatingPartTransform = translationBack * rotation * translationToCenter * rotatingPartTransform;
}
