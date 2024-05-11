//
// Created by Jacob Edwards on 11/05/2024.
//

#include "Barriers.h"


#include "utils/ShaderManager.h"
#include "Config.h"
#include "renderables/Entity.h"


Barriers::Barriers() : model("../Assets/objects/barrier/Concrete-Barrier_04.obj") {
    shader = ShaderManager::GetInstance().get("Base");
    // draw along the above
    // left transforms

    auto leftPos = glm::vec3(-103.0F, -6.0F, -82.0F);
    for (int i = 0; i < 5; i++) {
        glm::mat4 transform = Config::IDENTITY_MATRIX;
        transform = glm::translate(transform, leftPos);
        transform = glm::scale(transform, glm::vec3(15.0F));
        transform = glm::rotate(transform, glm::radians(90.0F), glm::vec3(0.0F, 1.0F, 0.0F));
        transforms.push_back(transform);
        leftPos.z += 42.0F;
    }

    auto backPos = glm::vec3(-82.0F, -6.0F, -103.0F);
    for (int i = 0; i < 5; i++) {
        glm::mat4 transform = Config::IDENTITY_MATRIX;
        transform = glm::translate(transform, backPos);
        transform = glm::scale(transform, glm::vec3(15.0F));
        transforms.push_back(transform);
        backPos.x += 43.0F;
    }

    auto rightPos = glm::vec3(103.0F, -6.0F, -81.0F);
    for (int i = 0; i < 5; i++) {
        glm::mat4 transform = Config::IDENTITY_MATRIX;
        transform = glm::translate(transform, rightPos);
        transform = glm::scale(transform, glm::vec3(15.0F));
        transform = glm::rotate(transform, glm::radians(90.0F), glm::vec3(0.0F, 1.0F, 0.0F));
        transforms.push_back(transform);
        rightPos.z += 42.0F;
    }
    auto frontPos = glm::vec3(-77.0F, -6.0F, 103.0F);
    for (int i = 0; i < 5; i++) {
        glm::mat4 transform = Config::IDENTITY_MATRIX;
        transform = glm::translate(transform, frontPos);
        transform = glm::scale(transform, glm::vec3(15.0F));
        transforms.push_back(transform);
        frontPos.x += 42.0F;
    }
}

void Barriers::draw(const std::shared_ptr<Shader> shader) const {
    // draw lots of walls surrounding
    for (const auto transform: transforms) {
        shader->setUniform("model", transform);
        model.draw(shader);
    }
}

