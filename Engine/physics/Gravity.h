#ifndef GRAVITY_H
#define GRAVITY_H

#include <glm/glm.hpp>

namespace Physics {
    constexpr float GRAVITY = 9.8F;
    constexpr float TERMINAL_VELOCITY = 53.0F;
    constexpr glm::vec3 GRAVITY_VECTOR = glm::vec3(0.0F, -GRAVITY, 0.0F);

// formulas
    auto calculateFallDistance(float time) -> float;

    auto calculateVelocityFloat(float time) -> float;

    auto calculateTime(float distance) -> float;

    auto calculateImpactVelocity(float distance) -> float;

    auto calculateVelocity(float time) -> glm::vec3;

    auto calculateVelocity(float time, glm::vec3 initialVelocity) -> glm::vec3;

} // namespace Physics

#endif // GRAVITY_H
