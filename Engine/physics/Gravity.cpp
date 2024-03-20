#include "physics/Gravity.h"

auto Physics::calculateFallDistance(float time) -> float {
    return 0.5F * GRAVITY * time * time;
}

auto Physics::calculateVelocityFloat(float time) -> float {
    return GRAVITY * time;
}

auto Physics::calculateTime(float distance) -> float {
    return std::sqrt(2 * distance / GRAVITY);
}

auto Physics::calculateImpactVelocity(float distance) -> float {
    return std::sqrt(2 * GRAVITY * distance);
}

auto Physics::calculateVelocity(float time) -> glm::vec3 {
    return {0.0F, -GRAVITY * time, 0.0F};
}

auto Physics::calculateVelocity(float time, glm::vec3 initialVelocity)
-> glm::vec3 {
    return initialVelocity + calculateVelocity(time);
}
