//
// Created by Jacob Edwards on 28/03/2024.
//

#ifndef CW_ENTITY_H
#define CW_ENTITY_H

#include <glm/ext/vector_float3.hpp>
#include <memory>
#include <glm/ext/matrix_float4x4.hpp>
#include <filesystem>
#include <vector>
#include "utils/Shader.h"
#include "utils/BoundingBox.h"
#include "graphics/Model.h"
#include "physics/ModelAttributes.h"
#include "graphics/Renderable.h"

class Entity : public Renderable {
public:
    using Renderable::draw;

    Entity() = default;

    explicit Entity(const std::filesystem::path &path);

    ~Entity() override = default;

    Entity(const Entity &other) = delete;

    auto operator=(const Entity &other) -> Entity & = delete;

    Entity(Entity &&other) noexcept = default;

    auto operator=(Entity &&other) noexcept -> Entity & = default;

    virtual void update(float deltaTime);

    void draw(const glm::mat4 &view, const glm::mat4 &projection) const override;

    void draw(std::shared_ptr<Shader> shader) const override;

    [[nodiscard]] auto getModel() const -> const Model &;

    [[nodiscard]] auto getBoundingBox() const -> const BoundingBox &;

    [[nodiscard]] auto getAttributes() -> Physics::Attributes &;

    void setBoundingBox(const BoundingBox &box);

    void setModel(std::unique_ptr<Model> model);

    void setAttributes(const Physics::Attributes &attributes);

    void translate(const glm::vec3 &translation);

    void transform(const glm::mat4 &transformation);

    void scale(const glm::vec3 &scale);

    void rotate(const glm::vec3 &axis, float angle);

    Physics::Attributes attributes;

protected:
    std::unique_ptr<Model> model;
    BoundingBox box;

    std::vector<std::unique_ptr<Entity> > children;
    Entity *parent = nullptr;
};


#endif //CW_ENTITY_H
