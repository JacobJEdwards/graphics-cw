//
// Created by Jacob Edwards on 27/02/2024.
//

#ifndef SUN_H
#define SUN_H

#include <GL/glew.h>
#include "Entity.h"
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float3.hpp>

class Sun : public Entity {
public:
    Sun();

    void update(float deltaTime) override;

    void draw(const glm::mat4 &view, const glm::mat4 &projection) const override;

    void setPosition(const glm::vec3 &pos);

    [[nodiscard]] auto getDirection() const -> glm::vec3;

    void setScale(float newScale);

    [[nodiscard]] auto getPosition() const -> glm::vec3;

    void interface();

private:
    float scale = 0.005F;
    float angle = 0.0F;
    bool move = true;
    float speed = 1.0F;

    Entity moon;
};

#endif //SUN_H