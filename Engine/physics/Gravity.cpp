#include "physics/Gravity.h"
#include <glm/ext/vector_float3.hpp>
#include <cmath>

auto Physics::calculateFallDistance(const float time) -> float {
    return 0.5F * GRAVITY * time * time;
}

auto Physics::calculateVelocityFloat(const float time) -> float {
    return GRAVITY * time;
}

auto Physics::calculateTime(const float distance) -> float {
    return std::sqrt(2 * distance / GRAVITY);
}

auto Physics::calculateImpactVelocity(const float distance) -> float {
    return std::sqrt(2 * GRAVITY * distance);
}

auto Physics::calculateVelocity(const float time) -> glm::vec3 {
    return {0.0F, -GRAVITY * time, 0.0F};
}

auto Physics::calculateVelocity(const float time, const glm::vec3 initialVelocity)
-> glm::vec3 {
    return initialVelocity + calculateVelocity(time);
}
