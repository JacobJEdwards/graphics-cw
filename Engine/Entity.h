//
// Created by Jacob Edwards on 28/03/2024.
//

#ifndef CW_ENTITY_H
#define CW_ENTITY_H

#include <glm/glm.hpp>
#include <utility>
#include "utils/Shader.h"
#include "utils/BoundingBox.h"
#include "utils/ShaderManager.h"
#include "graphics/Model.h"
#include "physics/ModelAttributes.h"

class Entity {
public:
    Entity() = default;

    Entity(const std::filesystem::path &path);

    virtual ~Entity() = default;

    Entity(const Entity &other) = delete;

    auto operator=(const Entity &other) -> Entity & = delete;

    Entity(Entity &&other) noexcept = default;

    auto operator=(Entity &&other) noexcept -> Entity & = default;


    virtual void update(float deltaTime);

    virtual void draw(const glm::mat4 &view, const glm::mat4 &projection) const;

    virtual void draw(std::shared_ptr<Shader> shader) const;

    [[nodiscard]] auto getModel() const -> const Model &;

    [[nodiscard]] auto getShader() const -> std::shared_ptr<Shader>;

    [[nodiscard]] auto getBoundingBox() -> BoundingBox &;

    [[nodiscard]] auto getAttributes() -> Physics::Attributes &;

    void setBoundingBox(const BoundingBox &box);

    void setModel(std::unique_ptr<Model> model);

    void setShader(const std::shared_ptr<Shader> &shader);

    void setAttributes(const Physics::Attributes &attributes);

    void translate(const glm::vec3 &translation);

    void transform(const glm::mat4 &transformation);

    void scale(const glm::vec3 &scale);

    void rotate(const glm::vec3 &axis, float angle);

    Physics::Attributes attributes;

protected:
    std::unique_ptr<Model> model;
    std::shared_ptr<Shader> shader;
    BoundingBox box;


    std::vector<std::unique_ptr<Entity>> children = {};
    Entity *parent = nullptr;

};


#endif //CW_ENTITY_H
