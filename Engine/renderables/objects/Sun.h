//
// Created by Jacob Edwards on 27/02/2024.
//

#ifndef SUN_H
#define SUN_H

#include "renderables/Entity.h"
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float3.hpp>

class Sun final : public Entity {
public:
    using Entity::draw;

    Sun();

    void update(float deltaTime) override;

    void draw(const glm::mat4 &view, const glm::mat4 &projection) const override;

    void setPosition(const glm::vec3 &pos);

    [[nodiscard]] auto getDirection() const -> glm::vec3;

    void setScale(float newScale);

    [[nodiscard]] auto getPosition() const -> glm::vec3;

    [[nodiscard]] auto getDiffuse() const -> glm::vec3;

    [[nodiscard]] auto getSpecular() const -> glm::vec3;

    [[nodiscard]] auto getAmbient() const -> glm::vec3;

    void interface();

private:
    float scale = 0.05F;
    float angle = 2.0F;
    bool move = true;
    float speed = 1.0F;

    float colorMixFactor = 0.0F;
    float darknessFactor = 0.0F;

    Entity moon;
};

#endif //SUN_H
