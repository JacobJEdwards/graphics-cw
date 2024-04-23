//
// Created by Jacob Edwards on 28/03/2024.
//

#include "renderables/Entity.h"


#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <memory>
#include <filesystem>
#include <unordered_map>
#include <utility>
#include "graphics/Shader.h"
#include "utils/BoundingBox.h"
#include "graphics/Model.h"
#include "physics/ModelAttributes.h"
#include "App.h"

std::unordered_map<std::filesystem::path, std::shared_ptr<Model> > Entity::models;

Entity::Entity(const std::filesystem::path &path) {
    if (const auto it = models.find(path); it != models.end()) {
        model = it->second;
    } else {
        model = std::make_shared<Model>(path);
        models.emplace(path, model);
    }

    box = model->getBoundingBox();
}

void Entity::update(const float deltaTime) {
    const glm::mat4 oldTransform = attributes.transform;
    attributes.update(deltaTime);
    const glm::mat4 newTransform = attributes.transform;

    const auto translation = glm::vec3(newTransform[3]) - glm::vec3(oldTransform[3]);

    box.translate(translation);
}

void Entity::draw(const glm::mat4 &view, const glm::mat4 &projection) const {
    shader->use();
    shader->setUniform("view", view);
    shader->setUniform("projection", projection);

    draw(shader);

    if (App::debug) {
        box.draw(attributes.transform, view, projection);
    }
}


void Entity::draw(const std::shared_ptr<Shader> shader) const {
    shader->use();
    shader->setUniform("model", attributes.transform);
    model->draw(shader);
}


[[nodiscard]] auto Entity::getModel() const -> const Model & {
    return *model;
}

[[nodiscard]] auto Entity::getBoundingBox() const -> const BoundingBox & {
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

void Entity::rotate(const glm::vec3 &axis, const float angle) {
    attributes.applyRotation(axis * angle);
    box.rotate(axis, angle);
}

void Entity::collisionResponse() {
}
