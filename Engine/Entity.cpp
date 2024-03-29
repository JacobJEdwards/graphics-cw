//
// Created by Jacob Edwards on 28/03/2024.
//

#include "Entity.h"


#include <glm/glm.hpp>
#include <memory>
#include <filesystem>
#include <utility>
#include "utils/Shader.h"
#include "utils/BoundingBox.h"
#include "utils/ShaderManager.h"
#include "graphics/Model.h"
#include "physics/ModelAttributes.h"
#include "App.h"

Entity::Entity(const std::filesystem::path &path) {
    model = std::make_unique<Model>(path);
    box = model->getBoundingBox();
    shader = ShaderManager::Get("Base");
}

void Entity::update(float deltaTime) {
    const glm::mat4 oldTransform = attributes.transform;
    attributes.update(deltaTime);
    const glm::mat4 newTransform = attributes.transform;

    auto translation = glm::vec3(newTransform[3]) - glm::vec3(oldTransform[3]);

    box.translate(translation);
}

void Entity::draw(const glm::mat4 &view, const glm::mat4 &projection) {
    shader->use();
    shader->setUniform("view", view);
    shader->setUniform("projection", projection);
    shader->setUniform("model", attributes.transform);

    model->draw(shader);

    if (App::debug) {
        box.draw(attributes.transform, view, projection);
    }
}


void Entity::draw(std::shared_ptr<Shader> shader) const {
    shader->use();
    shader->setUniform("model", attributes.transform);
    model->draw(shader);
}


[[nodiscard]] auto Entity::getModel() const -> const Model & {
    return *model;
}

[[nodiscard]] auto Entity::getBoundingBox() -> BoundingBox & {
    return box;
}

[[nodiscard]] auto Entity::getAttributes() -> Physics::Attributes & {
    return attributes;
}

void Entity::setBoundingBox(const BoundingBox &box) {
    this->box = box;
}

void Entity::setModel(std::unique_ptr<Model> model) {
    this->model = std::move(model);
    box = this->model->getBoundingBox();
}

void Entity::setAttributes(const Physics::Attributes &attributes) {
    this->attributes = attributes;
}

void Entity::translate(const glm::vec3 &translation) {
    attributes.position += translation;
    attributes.transform = glm::translate(attributes.transform, translation);

    box.translate(translation);
}

void Entity::transform(const glm::mat4 &transformation) {
    attributes.transform = transformation * attributes.transform;
    attributes.position =
            glm::vec3(attributes.transform * glm::vec4(attributes.position, 1.0F));
    box.transform(transformation);

}

void Entity::scale(const glm::vec3 &scale) {
    attributes.transform = glm::scale(attributes.transform, scale);
    box.scale(scale);
}

void Entity::rotate(const glm::vec3 &axis, float angle) {
    attributes.applyRotation(axis * angle);
    box.rotate(axis, angle);
}
