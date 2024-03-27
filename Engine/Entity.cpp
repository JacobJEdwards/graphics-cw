//
// Created by Jacob Edwards on 27/03/2024.
//

#include "Entity.h"
#include <memory>
#include <utility>
#include "graphics/Model.h"
#include "utils/Shader.h"
#include "physics/ModelAttributes.h"


Entity::Entity(std::shared_ptr<Model> model, const Physics::Attributes &attributes) {
    this->model = std::move(model);
    this->attributes = attributes;
}

void Entity::setModel(std::shared_ptr<Model> model) {
    this->model = model;
}

void Entity::setShader(std::shared_ptr<Shader> shader) {
    this->shader = shader;

    if (model) {
        model->setShader(shader);
    }
}

void Entity::setAttributes(const Physics::Attributes &attributes) {
    this->attributes = attributes;
}

auto Entity::getModel() -> std::shared_ptr<Model> {
    return model;
}

auto Entity::getShader() -> std::shared_ptr<Shader> {
    return shader;
}

auto Entity::getAttributes() -> Physics::Attributes {
    return attributes;
}

void Entity::draw(glm::mat4 view, glm::mat4 projection) {
    if (model) {
        model->draw(view, projection);
    }

    for (auto &child: children) {
        child->draw(view, projection);
    }
}

void Entity::update(float deltaTime) {
    attributes.update(deltaTime);

    for (auto &child: children) {
        child->update(deltaTime);
    }
}

void Entity::addChild(std::unique_ptr<Entity> child) {
    if (child) {
        child->setParent(std::unique_ptr<Entity>(this));
        children.push_back(std::move(child));
    }
}

template<typename... Args>
void Entity::addChild(Args &&... args) {
    children.push_back(std::make_unique<Entity>(std::forward<Args>(args)...));
    children.back()->setParent(std::unique_ptr<Entity>(this));
}

void Entity::setParent(std::unique_ptr<Entity> parent) {
    if (parent) {
        parent->addChild(std::unique_ptr<Entity>(this));
        this->parent = std::move(parent);
    }
}
