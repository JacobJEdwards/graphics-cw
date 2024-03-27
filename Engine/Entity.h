//
// Created by Jacob Edwards on 27/03/2024.
//

#ifndef CW_ENTITY_H
#define CW_ENTITY_H

#include <glm/glm.hpp>
#include "graphics/Model.h"
#include "utils/Shader.h"
#include "physics/ModelAttributes.h"

#include <vector>
#include <memory>

class Entity {
public:
    Entity() = default;

    Entity(std::shared_ptr<Model> model, const Physics::Attributes &attributes);

    void setModel(std::shared_ptr<Model> model);

    void setShader(std::shared_ptr<Shader> shader);

    void setAttributes(const Physics::Attributes &attributes);

    auto getModel() -> std::shared_ptr<Model>;

    auto getShader() -> std::shared_ptr<Shader>;

    auto getAttributes() -> Physics::Attributes;

    void draw(glm::mat4 view, glm::mat4 projection);

    void update(float deltaTime);

    void addChild(std::unique_ptr<Entity> child);

    template<typename... Args>
    void addChild(Args &&... args);

    void setParent(std::unique_ptr<Entity> parent);


private:
    Physics::Attributes attributes;
    std::shared_ptr<Model> model;
    std::shared_ptr<Shader> shader;

    std::vector<std::unique_ptr<Entity>> children;
    std::unique_ptr<Entity> parent = nullptr;
};

#endif //CW_ENTITY_H
