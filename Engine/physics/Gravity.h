#ifdef GRAVITY_H
#define GRAVITY_H

#include <cmath>
#include <glm/glm.hpp>

namespace Physics {
constexpr float GRAVITY = 9.8f;

// formulas
auto calculateFallDistance(float time) -> float {
  return 0.5f * GRAVITY * time * time;
}

auto calculateVelocityFloat(float time) -> float { return GRAVITY * time; }

auto calculateTime(float distance) -> float {
  return std::sqrt(2 * distance / GRAVITY);
}

auto calculateImpactVelocity(float distance) -> float {
  return std::sqrt(2 * GRAVITY * distance);
}

auto calculateVelocity(float time) -> glm::vec3 {
  return glm::vec3(0.0f, -GRAVITY * time, 0.0f);
}

auto calculateVelocity(float time, glm::vec3 initialVelocity) -> glm::vec3 {
  return initialVelocity + calculateVelocity(time);
}

} // namespace Physics

#endif // GRAVITY_H
