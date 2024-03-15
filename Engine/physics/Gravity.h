#ifndef GRAVITY_H
#define GRAVITY_H

#include <glm/glm.hpp>

namespace Physics {
constexpr float GRAVITY = 9.8f;
constexpr float TERMINAL_VELOCITY = 53.0f;
constexpr glm::vec3 GRAVITY_VECTOR = glm::vec3(0.0f, -GRAVITY, 0.0f);

// formulas
auto calculateFallDistance(float time) -> float;

auto calculateVelocityFloat(float time) -> float;

auto calculateTime(float distance) -> float;

auto calculateImpactVelocity(float distance) -> float;

auto calculateVelocity(float time) -> glm::vec3;

auto calculateVelocity(float time, glm::vec3 initialVelocity) -> glm::vec3;

} // namespace Physics

#endif // GRAVITY_H
